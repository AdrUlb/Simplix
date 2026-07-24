#include "Cpu.hpp"
#include "Runtime.hpp"
#include "Serial.hpp"
#include "Subsystems.hpp"

namespace Kernel
{
	extern "C" NORETURN void Main()
	{
		Runtime::Init();
		Subsystems::InitAll();

		while (true)
			Cpu::WaitInterrupts();
	}
}
