#include "Cpu.hpp"
#include "Runtime.hpp"
#include "Serial.hpp"

namespace Kernel
{
	extern "C" NORETURN void Main()
	{
		Runtime::Init();
		Serial::Init(Serial::PORT_COM1);

		for (auto str = "Hello, World!\n"; *str; str++)
			Serial::Send(Serial::PORT_COM1, *str);

		while (true)
			Cpu::WaitInterrupts();
	}
}
