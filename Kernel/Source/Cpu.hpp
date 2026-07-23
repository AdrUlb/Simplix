#pragma once
#include <cstddef>
#include <cstdint>
#include <utility>

#include "Silly/Macros.hpp"

namespace Kernel::Cpu
{
#if defined(__x86_64__) || defined(__i386__)
	FORCE_INLINE void Pause() { asm volatile("pause" ::: "memory"); }

	FORCE_INLINE size_t GetFlags()
	{
		size_t flags = 0;
		asm volatile(
			"pushf\n"
			"pop %0"
			: "=r"(flags)
		);
		return flags;
	}

	FORCE_INLINE bool GetInterruptsEnabled() { return GetFlags() & (1 << 9); }
	FORCE_INLINE void EnableInterrupts() { asm volatile("sti" ::: "memory"); }
	FORCE_INLINE void DisableInterrupts() { asm volatile("cli": : : "memory"); }
	FORCE_INLINE void WaitInterrupts() { asm volatile("hlt": : : "memory"); }

#elif defined(__aarch64__)
	FORCE_INLINE void Pause() { asm volatile("yield" ::: "memory"); }
#else
#error Your platform is not supported.
#endif

	class IrqGuard
	{
		enum class Action : uint8_t
		{
			None,
			Enable,
			Disable
		};

	public:
		IrqGuard(const IrqGuard&) = delete;
		IrqGuard& operator=(const IrqGuard&) = delete;

		IrqGuard(IrqGuard&& other) noexcept : _action(std::exchange(other._action, Action::None)) {}

		IrqGuard& operator=(IrqGuard&& other) noexcept
		{
			if (&other == this)
				return *this;

			Restore();
			_action = std::exchange(other._action, Action::None);

			return *this;
		}

		~IrqGuard()
		{
			Restore();
		}

		static IrqGuard Enable()
		{
			const auto action = GetInterruptsEnabled() ? Action::Enable : Action::Disable;
			EnableInterrupts();
			return IrqGuard { action };
		}

		static IrqGuard Disable()
		{
			const auto action = GetInterruptsEnabled() ? Action::Enable : Action::Disable;
			DisableInterrupts();
			return IrqGuard { action };
		}

	private:
		void Restore()
		{
			switch (_action)
			{
				case Action::None:
					break;
				case Action::Enable:
					EnableInterrupts();
					break;
				case Action::Disable:
					DisableInterrupts();
					break;
			}

			_action = Action::None;
		}

		explicit IrqGuard(const Action action) : _action(action) {}

		Action _action;
	};
}
