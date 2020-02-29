#pragma once
#include <array>
#include <cstdint>
#include <functional>
#include <iomanip>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "llvmes/interpreter/instruction.h"
#include "llvmes/interpreter/status_register.h"

namespace llvmes {

using DisassemblyMap = std::map<std::uint16_t, std::string>;
typedef std::function<std::uint8_t(std::uint16_t)> BusRead;
typedef std::function<void(std::uint16_t, std::uint8_t)> BusWrite;

class CPU {
   public:
    CPU();
    void Step();
    void Run();
    void Reset();
    void Dump();
    void SetNMI();
    void SetIRQ();
    DisassemblyMap Disassemble(std::uint16_t start, std::uint16_t stop);

    BusRead Read;
    BusWrite Write;

    std::uint8_t reg_x;
    std::uint8_t reg_y;
    std::uint8_t reg_a;
    std::uint8_t reg_sp;
    std::uint16_t reg_pc;
    StatusRegister reg_status;

   private:
    constexpr static unsigned int FLAG_C = (1 << 0);
    constexpr static unsigned int FLAG_Z = (1 << 1);
    constexpr static unsigned int FLAG_I = (1 << 2);
    constexpr static unsigned int FLAG_D = (1 << 3);
    constexpr static unsigned int FLAG_B = (1 << 4);
    constexpr static unsigned int FLAG_UNUSED = (1 << 5);
    constexpr static unsigned int FLAG_V = (1 << 6);
    constexpr static unsigned int FLAG_N = (1 << 7);

    constexpr static unsigned int NMI_VECTOR = 0xFFFA;
    // Address at this location points to the first instruction
    constexpr static unsigned int RESET_VECTOR = 0xFFFC;
    constexpr static unsigned int IRQ_VECTOR = 0xFFFE;

   private:
    std::vector<Instruction> m_instruction_table;
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

template <typename T>
inline std::string ToHexString(T i)
{
    std::stringstream stream;
    stream << "$" << std::uppercase << std::setfill('0')
           << std::setw(sizeof(T) * 2) << std::hex << (unsigned)i;
    return stream.str();
}

template <>
inline std::string ToHexString<bool>(bool i)
{
    std::stringstream stream;
    stream << std::uppercase << std::setw(1) << std::hex << i;
    return stream.str();
}

inline int HexStringToInt(const std::string& in)
{
    auto strip_zeroes = [](std::string temp) {
        while (temp.at(0) == '0') temp = temp.substr(1);
        return std::move(temp);
    };

    std::string out;
    out = (in.at(0) == '$') ? strip_zeroes(in.substr(1)) : strip_zeroes(in);
    return std::stoi(out, 0, 16);
}

}  // namespace llvmes
