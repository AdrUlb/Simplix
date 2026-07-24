#pragma once
#include <cpuid.h>
#include <cstdint>

#include "Silly/Macros.hpp"

namespace Kernel::X86
{
	static constexpr uint32_t MSR_EFER = 0xC0000080;
	static constexpr uint32_t MSR_FSBASE = 0xC0000100;
	static constexpr uint32_t MSR_GSBASE = 0xC0000101;

	static void CpuId(const uint32_t level, uint32_t* a, uint32_t* b, uint32_t* c, uint32_t* d)
	{
		uint32_t ignore;

		if (!a)
			a = &ignore;
		if (!b)
			b = &ignore;
		if (!c)
			c = &ignore;
		if (!d)
			d = &ignore;

		__cpuid(level, a, b, c, d);
	}

	[[nodiscard]] static uint64_t GetTimestampCounter()
	{
		uint64_t value;
		asm volatile("rdtsc" : "=A"(value));
		return value;
	}

	[[nodiscard]] static uint64_t ReadMsr(const uint32_t index)
	{
		uint64_t value;
		asm volatile("rdmsr" : "=A"(value) : "c"(index));
		return value;
	}

	static void WriteMsr(const uint32_t index, const uint64_t value)
	{
		asm volatile("wrmsr" : : "A"(value), "c"(index));
	}

	namespace Ports
	{
		static FORCE_INLINE void Out8(const uint16_t port, const uint8_t data)
		{
			asm volatile("out %[port], %[data]" : : [data]"a"(data), [port]"Nd"(port));
		}

		static FORCE_INLINE void Out16(const uint16_t port, const uint16_t data)
		{
			asm volatile("out %[port], %[data]" : : [data]"a"(data), [port]"Nd"(port));
		}

		static FORCE_INLINE void Out32(const uint16_t port, const uint32_t data)
		{
			asm volatile("out %[port], %[data]" : : [data]"a"(data), [port]"Nd"(port));
		}

		[[nodiscard]] static FORCE_INLINE uint8_t In8(const uint16_t port)
		{
			uint8_t data;
			asm volatile("in %[data], %[port]" : [data]"=a"(data) : [port]"Nd"(port));
			return data;
		}

		[[nodiscard]] static FORCE_INLINE uint16_t In16(const uint16_t port)
		{
			uint16_t data;
			asm volatile("in %[data], %[port]" : [data]"=a"(data) : [port]"Nd"(port));
			return data;
		}

		[[nodiscard]] static FORCE_INLINE uint32_t In32(const uint16_t port)
		{
			uint32_t data;
			asm volatile("in %[data], %[port]" : [data]"=a"(data) : [port]"Nd"(port));
			return data;
		}

		static void Delay()
		{
			Out8(0x80, 0);
		}
	}
}
