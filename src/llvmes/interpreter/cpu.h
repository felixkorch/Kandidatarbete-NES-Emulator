#pragma once
#include "llvmes/interpreter/status_register.h"
#include "llvmes/interpreter/instruction.h"

#include <cstdint>
#include <vector>
#include <array>
#include <string>
#include <functional>

namespace llvmes {

    class CPU {
    public:
        typedef std::function<std::uint8_t(std::uint16_t)> BusRead;
        typedef std::function<void(std::uint16_t, std::uint8_t)> BusWrite;

        CPU();
        void Step();
        void Run();
        void Reset();
        void Dump();
        void SetNMI();
        void SetIRQ();

        BusRead Read;
        BusWrite Write;

        std::uint8_t   regX;
        std::uint8_t   regY;
        std::uint8_t   regA;
        std::uint8_t   regSP;
        std::uint16_t  regPC;
        StatusRegister regStatus;
        std::vector<Instruction> instructionTable;

	private:

        constexpr static unsigned int FLAG_C      = (1 << 0);
        constexpr static unsigned int FLAG_Z      = (1 << 1);
        constexpr static unsigned int FLAG_I      = (1 << 2);
        constexpr static unsigned int FLAG_D      = (1 << 3);
        constexpr static unsigned int FLAG_B      = (1 << 4);
        constexpr static unsigned int FLAG_UNUSED = (1 << 5);
        constexpr static unsigned int FLAG_V      = (1 << 6);
        constexpr static unsigned int FLAG_N      = (1 << 7);

        constexpr static unsigned int NMI_VECTOR   = 0xFFFA;
        // Address at this location points to the first instruction
        constexpr static unsigned int RESET_VECTOR = 0xFFFC;
        constexpr static unsigned int IRQ_VECTOR   = 0xFFFE;

    private:
        bool m_irq, m_nmi;

        // Will be set to true whenever an illegal op-code gets fetched
        bool m_illegal_opcode;
        // Contains the address associated with an instruction
        std::uint16_t m_address;
    private:

        /// Does two consecutively reads at a certain address
        std::uint16_t Read16(std::uint16_t addr);

        void StackPush(std::uint8_t value);
        std::uint8_t StackPop();

        void InvokeIRQ();
        void InvokeNMI();

        void AddressModeImmediate();
        void AddressModeAbsolute();
        void AddressModeAbsoluteX();
        void AddressModeAbsoluteY();
        void AddressModeZeropage();
        void AddressModeZeropageX();
        void AddressModeZeropageY();
        void AddressModeIndirect();
        void AddressModeIndirectX();
        void AddressModeIndirectY();
        void AddressModeImplied();
        void AddressModeAccumulator();

        void OP_BIT();
        void OP_AND();
        void OP_EOR();
        void OP_ORA();
        void OP_ASL();
        void OP_ASL_ACC();
        void OP_LSR();
        void OP_LSR_ACC();
        void OP_ADC();
        void OP_JSR();
        void OP_JMP();
        void OP_BNE();
        void OP_BEQ();
        void OP_BMI();
        void OP_BCC();
        void OP_BCS();
        void OP_BPL();
        void OP_BVC();
        void OP_BVS();
        void OP_BRK();
        void OP_LDY();
        void OP_LDA();
        void OP_LDX();
        void OP_INX();
        void OP_INC();
        void OP_DEC();
        void OP_INY();
        void OP_DEY();
        void OP_DEX();
        void OP_NOP();
		void OP_SEI();
		void OP_CLI();
        void OP_CLC();
        void OP_CLD();
        void OP_CLV();
		void OP_PHA();
		void OP_PHP();
		void OP_PLA();
		void OP_PLP();
		void OP_ROL();
		void OP_ROR();
        void OP_ROL_ACC();
		void OP_ROR_ACC();
		void OP_RTI();
		void OP_RTS();
		void OP_SBC();
		void OP_SEC();
		void OP_SED();
		void OP_STA();
		void OP_STX();
		void OP_STY();
		void OP_TAX();
		void OP_TAY();
		void OP_TSX();
		void OP_TYA();
		void OP_TXS();
		void OP_TXA();
		void OP_CMP();
		void OP_CPX();
		void OP_CPY();
        void IllegalOP();
    };

}
