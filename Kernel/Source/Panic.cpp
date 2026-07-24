#include <cstdlib>

#include "Cpu.hpp"
#include "Silly/Extern.hpp"
#include "Silly/Macros.hpp"

NEVER_INLINE RARELY_USED void Silly::Extern::VerifyFailed(const char* condition, const std::source_location location)
{
	using namespace Kernel;

	// TODO
	IGNORE(condition);
	IGNORE(location);
	/*
	std::print(stderr,
			   "A fatal error occured: {}\n"
			   "  in {}:{}:{}\n"
			   "  function: {}\n",
			   condition,
			   location.file_name(), location.line(), location.column(),
			   location.function_name());
	*/

	Cpu::DisableInterrupts();
	while (true)
		Cpu::WaitInterrupts();
}
