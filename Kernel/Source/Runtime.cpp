#include "Runtime.hpp"

#include <cstdint>

#include "Silly/Macros.hpp"

namespace Kernel::Runtime
{
	using InitFiniFunc = void(*)();

	extern InitFiniFunc initArray[] asm("INIT_ARRAY_START");
	extern InitFiniFunc initArrayEnd[] asm("INIT_ARRAY_END");

	extern InitFiniFunc finiArray[] asm("FINI_ARRAY_START");
	extern InitFiniFunc finiArrayEnd[] asm("FINI_ARRAY_END");

	void Init()
	{
		for (auto func = initArray; func < initArrayEnd; func++)
			(*func)();

		// Prevent the compiler from doing some silly reordering
		asm volatile("" : : : "memory");
	}

	void Fini()
	{
		asm volatile("" : : : "memory");

		for (auto func = finiArrayEnd; func > finiArray; func--)
			(*(func - 1))();
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
__attribute__((aligned(16))) static uint8_t failStack[16 * 1024];
static constexpr auto failStackBottom = failStack + sizeof(failStack);

static NEVER_INLINE RARELY_USED NORETURN void OnStackCheckFail()
{
	// TODO: message "stack smashing detected"
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
		"mov rsp, %0 - 8\n" // NOTE: the stack is purposefully offset to account for the return address that isn't pushed by the following jump
		"jmp %1\n" // OnStackCheckFail is NORETURN, we don't need to call
		:
		: "i"(failStackBottom), "i"(OnStackCheckFail)
		: "memory"
	);
#elif defined(__i386__)
	asm volatile(
		"cli\n"
		"mov esp, %0 - 4\n" // NOTE: the stack is purposefully offset to account for the return address that isn't pushed by the following jump
		"jmp %1\n" // OnStackCheckFail is NORETURN, we don't need to call
		:
		: "i"(failStackBottom), "i"(OnStackCheckFail)
		: "memory"
	);
#else
#error Your platform is not supported.
#endif
}
}
