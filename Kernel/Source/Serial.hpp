#pragma once
#include <bit>
#include <cstdint>

#include "Cpu.hpp"
#include "X86.hpp"
#include "Silly/Macros.hpp"

namespace Kernel::Serial
{
	static constexpr uint16_t PORT_COM1 = 0x3F8;
	static constexpr uint16_t PORT_COM2 = 0x2F8;

	static constexpr uint16_t REG_RECEIVE = 0;
	static constexpr uint16_t REG_TRANSMIT = 0;
	static constexpr uint16_t REG_INT_ENABLE = 1;
	static constexpr uint16_t REG_FIFO_CONTROL = 2;
	static constexpr uint16_t REG_LINE_CONTROL = 3;
	static constexpr uint16_t REG_MODEM_CONTROL = 4;
	static constexpr uint16_t REG_LINE_STATUS = 5;
	static constexpr uint16_t REG_DLAB_BAUD_DIV_LOW = 0;
	static constexpr uint16_t REG_DLAB_BAUD_DIV_HIGH = 1;

	enum class InterruptTriggerLevel : uint8_t
	{
		Bytes1,
		Bytes4,
		Bytes8,
		Bytes14,
	};

	enum class DataBits : uint8_t
	{
		Length5 = 0,
		Length6 = 1,
		Length7 = 2,
		Length8 = 3,
	};

	enum class StopBits : uint8_t
	{
		Length1,
		Length2
	};

	enum class ParityBits : uint8_t
	{
		None = 0, // Technically any value with the lowest bit set will result in no parity bits
		Odd = 1,
		Even = 3,
		Mark = 5,
		Space = 7,
	};

	struct PACKED InterruptEnable
	{
		bool ReceivedDataAvailable : 1 { };
		bool TransmitterHoldingRegisterEmpty : 1 { };
		bool ReceiverLineStatus : 1 { };
		bool ModemStatus : 1 { };
		bool  : 4; // Reserved
	};

	struct PACKED FifoControl
	{
		bool Enable : 1 { };
		bool ClearReceive : 1 { };
		bool ClearTransmit : 1 { };
		uint8_t DmaModeSelect : 1{ };
		uint8_t  : 2; // Reserved
		InterruptTriggerLevel InterruptTriggerLevel : 2{ };
	};

	struct PACKED LineControl
	{
		DataBits DataBits : 2 { };
		StopBits StopBits : 1 { };
		ParityBits ParityBits : 3 { };
		bool BreakEnable : 1 { };
		bool DivisorLatchAccess : 1 { };
	};

	struct PACKED ModemControl
	{
		bool DataTerminalReady : 1 { };
		bool RequestToSend : 1 { };
		bool  : 1; // PC-specific: unused
		bool EnableIrq : 1 { }; // PC-specific
		bool Loop : 1 { };
		bool  : 3; // Unused
	};

	struct PACKED LineStatus
	{
		bool DataReady : 1;
		bool OverrunError : 1;
		bool ParityError : 1;
		bool FramingError : 1;
		bool BreakIndicator : 1;
		bool TransmitterBufferEmpty : 1;
		bool TransmitterEmpty : 1;
		bool ImpendingError : 1;
	};

	static FORCE_INLINE InterruptEnable GetInterruptEnable(const uint16_t port)
	{
		return std::bit_cast<InterruptEnable>(X86::Ports::In8(port + REG_INT_ENABLE));
	}

	static FORCE_INLINE void SetFifo(const uint16_t port, const FifoControl value)
	{
		X86::Ports::Out8(port + REG_FIFO_CONTROL, std::bit_cast<uint8_t>(value));
	}

	static FORCE_INLINE void SetInterruptEnable(const uint16_t port, const InterruptEnable value)
	{
		X86::Ports::Out8(port + REG_INT_ENABLE, std::bit_cast<uint8_t>(value));
	}

	static FORCE_INLINE LineControl GetLineControl(const uint16_t port)
	{
		return std::bit_cast<LineControl>(X86::Ports::In8(port + REG_LINE_CONTROL));
	}

	static FORCE_INLINE void SetLineControl(const uint16_t port, const LineControl value)
	{
		X86::Ports::Out8(port + REG_LINE_CONTROL, std::bit_cast<uint8_t>(value));
	}

	static FORCE_INLINE ModemControl GetModemControl(const uint16_t port)
	{
		return std::bit_cast<ModemControl>(X86::Ports::In8(port + REG_MODEM_CONTROL));
	}

	static FORCE_INLINE void SetModemControl(const uint16_t port, const ModemControl value)
	{
		X86::Ports::Out8(port + REG_MODEM_CONTROL, std::bit_cast<uint8_t>(value));
	}

	static FORCE_INLINE LineStatus GetLineStatus(const uint16_t port)
	{
		return std::bit_cast<LineStatus>(X86::Ports::In8(port + REG_LINE_STATUS));
	}

	static FORCE_INLINE void SetLineStatus(const uint16_t port, const LineStatus value)
	{
		X86::Ports::Out8(port + REG_LINE_STATUS, std::bit_cast<uint8_t>(value));
	}

	static FORCE_INLINE bool CanSend(const uint16_t port)
	{
		return GetLineStatus(port).TransmitterBufferEmpty;
	}

	static FORCE_INLINE void Send(const uint16_t port, const uint8_t data)
	{
		while (!CanSend(port))
			Cpu::Pause();

		X86::Ports::Out8(port + REG_TRANSMIT, data);
	}

	static FORCE_INLINE void Send(const uint16_t port, const char data)
	{
		Send(port, static_cast<uint8_t>(data));
	}

	uint16_t GetBaudDivisor(uint16_t port);
	uint16_t SetBaudDivisor(uint16_t port, uint16_t value);

	void Init(uint16_t port);
}
