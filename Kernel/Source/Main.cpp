#include "Cpu.hpp"
#include "Runtime.hpp"

namespace Kernel
{
	extern "C" NORETURN void Main()
	{
		Runtime::Init();

		while (true)
			Cpu::WaitInterrupts();
	}
}
