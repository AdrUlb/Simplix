#pragma once
#include "Waiter.hpp"
#include "Silly/Threading/Mutex.hpp"
#include "Silly/Threading/ReaderWriterLock.hpp"

namespace Kernel::Sync
{
	template<typename T>
	concept Lockable = Threading::Lockable<T>;

	template<template <typename...> typename T>
	concept LockableGuard = Threading::LockableGuard<T>;

	// Copy LockGuard into the Kernel::Sync namespace
	template<Lockable T> using LockGuard = Threading::LockGuard<T>;

	// Define basic locks
	using SpinLock = Threading::Mutex<SpinWaiter>;
	using ReaderWriterSpinLock = Threading::ReaderWriterLock<SpinWaiter>;

	template<Lockable Lock>
	class IrqLockGuard
	{
		explicit IrqLockGuard(LockGuard<Lock>&& lockGuard, Cpu::IrqGuard&& irqGuard) noexcept
			: _irqGuard(std::move(irqGuard)), _lock(std::move(lockGuard)) {}

	public:
		[[nodiscard]] static IrqLockGuard Take(Lock& lock)
		{
			return IrqLockGuard(LockGuard<Lock>::Take(lock), Cpu::IrqGuard::Disable());
		}

		[[nodiscard]] static Option<IrqLockGuard> TryTake(Lock& lock)
		{
			auto guard = LockGuard<Lock>::TryTake(lock);
			if (!guard)
				return None;

			return Some(IrqLockGuard(std::move(guard).Unwrap(), Cpu::IrqGuard::Disable()));
		}

	private:
		// NOTE: the order here is important: the lock and guard are released in *reverse order of declaration here*
		Cpu::IrqGuard _irqGuard;
		LockGuard<Lock> _lock;
	};

	static_assert(LockableGuard<IrqLockGuard>);
}
