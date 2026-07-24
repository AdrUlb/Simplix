#pragma once
#include <atomic>

#include "Silly/StringView.hpp"

namespace Kernel::Subsystems
{
	enum class InitScope
	{
		Global,
		PerCore
	};

	enum class InitState
	{
		Uninitialized,
		Initializing,
		Initialized,
	};

	using InitFunc = void(*)();

	struct InitInfo
	{
		const StringView Name;
		InitFunc Func;
		const StringView* Dependencies; // List of dependencies, terminated by empty view
		InitScope Scope;
		InitState State;
		std::atomic<uint64_t> CoreMask; // TODO: SMP: support more than 64 cores, maybe
	};

	void InitAll();
}

#define INIT_SUBSYSTEM_IMPL(name, func, scope, ...) \
	static constexpr StringView _subsys_##name##_deps_[] = { __VA_OPT__(__VA_ARGS__,) {} }; \
	static __attribute__((used, section(".init_subsystems"), aligned(alignof(Kernel::Subsystems::InitInfo)))) Kernel::Subsystems::InitInfo _subsys_##name##_ { \
		.Name = #name, \
		.Func = func, \
		.Dependencies = _subsys_##name##_deps_, \
		.Scope = scope, \
		.State = Kernel::Subsystems::InitState::Uninitialized, \
		.CoreMask = { 0 }, \
	};

#define INIT_SUBSYSTEM(name, func, ...) INIT_SUBSYSTEM_IMPL(name, func, Kernel::Subsystems::InitScope::Global, __VA_ARGS__)
#define INIT_SUBSYSTEM_PERCORE(name, func, ...) INIT_SUBSYSTEM_IMPL(name, func, Kernel::Subsystems::InitScope::PerCore, __VA_ARGS__)
