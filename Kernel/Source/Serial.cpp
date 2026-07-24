#include "Serial.hpp"

#include "Subsystems.hpp"

// https://wiki.osdev.org/Serial_Ports
namespace Kernel::Serial
{
	uint16_t GetBaudDivisor(const uint16_t port)
	{
		const auto oldLineControl = GetLineControl(port);

		auto newLineControl = oldLineControl;
		newLineControl.DivisorLatchAccess = true;
		SetLineControl(port, newLineControl);
		const auto low = X86::Ports::In8(port + REG_DLAB_BAUD_DIV_LOW);
		const auto high = X86::Ports::In8(port + REG_DLAB_BAUD_DIV_HIGH);

		if (!oldLineControl.DivisorLatchAccess)
			SetLineControl(port, oldLineControl);
		return (static_cast<uint16_t>(high) << 8) | low;
	}

	uint16_t SetBaudDivisor(const uint16_t port, const uint16_t value)
	{
		const auto oldLineControl = GetLineControl(port);

		auto newLineControl = oldLineControl;
		newLineControl.DivisorLatchAccess = true;
		SetLineControl(port, newLineControl);
		const auto low = value & 0xFF;
		const auto high = (value >> 8) & 0xFF;
		X86::Ports::Out8(port + REG_DLAB_BAUD_DIV_LOW, low);
		X86::Ports::Out8(port + REG_DLAB_BAUD_DIV_HIGH, high);

		if (!oldLineControl.DivisorLatchAccess)
			SetLineControl(port, oldLineControl);
		return (static_cast<uint16_t>(high) << 8) | low;
	}

	void Init(const uint16_t port)
	{
		// Set to 38400 baud (divisor = 3)
		static constexpr uint16_t baudDivisor = 3;

		static constexpr LineControl lineControl = {
			.DataBits = DataBits::Length8,
			.StopBits = StopBits::Length1,
			.ParityBits = ParityBits::None
		};

		static constexpr FifoControl fifoControl = {
			.Enable = true,
			.ClearReceive = true,
			.ClearTransmit = true,
			.InterruptTriggerLevel = InterruptTriggerLevel::Bytes14
		};

		static constexpr ModemControl modemControl = {
			.DataTerminalReady = true,
			.RequestToSend = true,
			.EnableIrq = true
		};

		SetInterruptEnable(port, { }); // Disable serial interrupts
		SetBaudDivisor(port, baudDivisor);
		SetLineControl(port, lineControl);
		SetFifo(port, fifoControl);
		SetModemControl(port, modemControl);
	}

	static void InitCom1()
	{
		Init(PORT_COM1);
	}

	static void InitCom2()
	{
		Init(PORT_COM2);
	}

	INIT_SUBSYSTEM(serial_com1, InitCom1);
	INIT_SUBSYSTEM(serial_com2, InitCom2);
}
