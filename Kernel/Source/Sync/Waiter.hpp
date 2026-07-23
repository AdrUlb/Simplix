#pragma once
#include <atomic>

#include "Silly/Threading/Mutex.hpp"

namespace Kernel::Sync
{
	template<typename T>
	concept Waiter = Threading::Waiter<T>;

	struct SpinWaiter
	{
		template<typename T> static void Wait(const std::atomic<T>&, const T, std::memory_order = std::memory_order_seq_cst) noexcept
		{
			Cpu::Pause();
		}

		template<typename T> static void NotifyOne(std::atomic<T>&) noexcept {}
		template<typename T> static void NotifyAll(std::atomic<T>&) noexcept {}
		static void Wait(const std::atomic_flag&, const bool, std::memory_order = std::memory_order_seq_cst) noexcept {}
		static void NotifyOne(std::atomic_flag&) noexcept {}
		static void NotifyAll(std::atomic_flag&) noexcept {}
	};

	static_assert(Waiter<SpinWaiter>);
}
