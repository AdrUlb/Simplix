#include "Cpu.hpp"
#include "Runtime.hpp"
#include "Sync/Lock.hpp"

namespace Kernel
{
	extern "C" NORETURN void Main()
	{
		Runtime::Init();

		// TODO

		while (true)
			Cpu::WaitInterrupts();
	}
}
