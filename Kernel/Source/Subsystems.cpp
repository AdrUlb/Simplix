#include "Subsystems.hpp"

#include "Silly/String.hpp"

namespace Kernel::Subsystems
{
	extern InitInfo subsystems[] asm("INIT_SUBSYSTEMS_START");
	extern InitInfo subsystemsEnd[] asm("INIT_SUBSYSTEMS_END");

	static Option<InitInfo&> TryFindSubsystem(const StringView name)
	{
		for (auto sub = subsystems; sub < subsystemsEnd; sub++)
		{
			if (sub->Name == name)
				return Some(*sub);
		}

		return None;
	}

	static void InitSubsystem(InitInfo& subsystem)
	{
		static constexpr auto coreBit = 1ULL << 0; // TODO: SMP: get actual core ID

		if (subsystem.Scope == InitScope::Global && subsystem.State == InitState::Initialized)
			return; // The subsystem has already been initialized

		// Relaxed memory order works fine because every CPU only cares about its own bit
		if (subsystem.Scope == InitScope::PerCore && subsystem.CoreMask.load(std::memory_order_relaxed) & coreBit)
			return; // The subsystem has already been initialized on this core

		if (subsystem.Scope == InitScope::Global)
		{
			// Expect the subsystem to be uninitialized, set it to initializing
			// If we encounter a dependency subsystem which is 'initializing' we have run into a dependency cycle
			if (subsystem.State != InitState::Uninitialized)
				VERIFY_NOT_REACHED("a dependency cycle was detected during subsystem initialization");

			subsystem.State = InitState::Initializing;
		}

		const auto deps = subsystem.Dependencies;

		for (size_t i = 0; !deps[i].IsEmpty(); i++)
		{
			auto& dep = TryFindSubsystem(deps[i]).Unwrap();
			if (dep.Scope == InitScope::PerCore && subsystem.Scope == InitScope::Global)
				VERIFY_NOT_REACHED("a global subsystem cannot depend on a per-core subsystem");

			InitSubsystem(dep);
		}

		subsystem.Func();

		if (subsystem.Scope == InitScope::Global)
		{
			subsystem.State = InitState::Initialized;
		}
		else
		{
			subsystem.CoreMask.fetch_or(coreBit, std::memory_order_relaxed);
		}
	}

	void InitAll()
	{
		for (auto sub = subsystems; sub < subsystemsEnd; sub++)
			InitSubsystem(*sub);

		// Prevent the compiler from doing some silly reordering
		asm volatile("" : : : "memory");
	}
}
