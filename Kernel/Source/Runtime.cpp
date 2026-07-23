#include "Runtime.hpp"

#include <cstdint>

#include "Cpu.hpp"
#include "Silly/Macros.hpp"

using InitFiniFunc = void(*)();

extern InitFiniFunc initArray[] asm("INIT_ARRAY_START");
extern InitFiniFunc initArrayEnd[] asm("INIT_ARRAY_END");

extern InitFiniFunc finiArray[] asm("INIT_ARRAY_START");
extern InitFiniFunc finiArrayEnd[] asm("INIT_ARRAY_END");

namespace Runtime
{
	void Init()
	{
		for (auto func = initArray; func < initArrayEnd; func++)
			(*func)();
	}

	void Fini()
	{
		for (auto func = finiArray; func < finiArrayEnd; func++)
			(*func)();
	}
}

extern "C" NEVER_INLINE RARELY_USED NORETURN void abort()
{
	VERIFY_NOT_REACHED();
}

#if UINT64_MAX == UINTPTR_MAX
#define STACK_CHK_GUARD_INIT 0x24680BDF000AFF0D
#elif UINT32_MAX == UINTPTR_MAX
#define STACK_CHK_GUARD_INIT 0x000AFF0D
#else
#error Your platform is not supported.
#endif

// Temporary stack, used on stack check failure to safely panic
__attribute__((aligned(16))) static uint8_t failStack[4096];

static NEVER_INLINE RARELY_USED NORETURN void OnStackCheckFail()
{
	VERIFY_NOT_REACHED();
}

extern "C"
{
USED uintptr_t __stack_chk_guard = STACK_CHK_GUARD_INIT; // NOLINT(*-reserved-identifier)
USED NAKED NORETURN void __stack_chk_fail() // NOLINT(*-reserved-identifier)
{
#if defined(__x86_64__)
	asm volatile(
		"cli\n"
		"mov rsp, %0\n"
		"call %1\n"
		"hlt"
		:
		: "i"(failStack + 4096), "i"(OnStackCheckFail)
		: "memory"
	);
#elif defined(__i386__)
	asm volatile(
		"cli\n"
		"mov rsp, %0\n"
		"call %1\n"
		"hlt"
		:
		: "i"(failStack + 4096), "i"(OnStackCheckFail)
		: "memory"
	);
#else
#error Your platform is not supported.
#endif
}
}
