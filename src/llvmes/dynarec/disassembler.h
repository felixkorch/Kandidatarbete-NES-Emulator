#pragma once

#include <iomanip>
#include <iostream>
#include <list>
#include <map>
#include <queue>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <cassert>

#include "llvmes/dynarec/6502_opcode.h"
#include "llvmes/common.h"


namespace llvmes {

enum class StatementType { DataWord, DataByte, Instruction, Label };

// Not used atm, probably will be later
enum class InstructionType {
    ImmediateInstruction,
    ImpliedInstruction,
    DirectWithLabelIndexedInstruction,
    DirectIndexedInstruction,
    DirectWithLabelInstruction,
    DirectInstruction,
    IndirectXInstruction,
    IndirectYInstruction,
    IndirectInstruction
};

struct Statement {
    template <class DeriveType>
    DeriveType& GetAs()
    {
        return (DeriveType&)*this;
    }

    template <class DeriveType>
    operator DeriveType&()
    {
        return (DeriveType&)*this;
    }

    virtual StatementType GetType() = 0;
    virtual void Print() = 0;
    virtual int GetOffset() = 0;
    virtual ~Statement() = default;
};

struct DataWord : public Statement {
    uint16_t offset = 0;
    uint16_t word = 0;

    constexpr DataWord(uint8_t a1, uint8_t a2) : word((a1 << 8) | a2) {}
    explicit constexpr operator uint16_t() const { return word; }
    explicit constexpr DataWord(uint16_t word) : word(word) {}

    StatementType GetType() override { return StatementType::DataWord; }

    void Print() override
    {
        std::cout << "[" << offset << "]: "
                  << ".dw " << ToHexString(word) << std::endl;
    }

    int GetOffset() override { return offset; }

    friend std::ostream& operator<<(std::ostream& os, const DataWord& dw);
};

inline std::ostream& operator<<(std::ostream& os, const DataWord& dw)
{
    os << ToHexString(dw.word);
    return os;
}

struct DataByte : public Statement {
    uint16_t offset = 0;
    uint8_t byte = 0;

    explicit constexpr operator uint8_t() const { return byte; }
    explicit constexpr DataByte(uint8_t a) : byte(a) {}

    StatementType GetType() override { return StatementType::DataByte; }

    void Print() override
    {
        std::cout << "[" << offset << "]: "
                  << ".db " << ToHexString(byte) << std::endl;
    }

    int GetOffset() override { return offset; }

    friend std::ostream& operator<<(std::ostream& os, const DataByte& db);
};

inline std::ostream& operator<<(std::ostream& os, const DataByte& db)
{
    os << ToHexString(db.byte);
    return os;
}

struct Instruction : public Statement {
    int size = 0;
    int offset = 0;
    uint8_t opcode = 0xFF;
    uint16_t arg = 0;
    std::string name;
    MOS6502::AddressingMode addressing_mode = {};
    MOS6502::Op op_type = {};

    bool is_branchinstruction = false;
    std::string target_label;

    StatementType GetType() override { return StatementType::Instruction; }

    void Print() override
    {
        std::cout << "[" << offset << "]: " << ToHexString(opcode) << std::endl;
    }

    int GetOffset() override { return offset; }
};

struct Label : public Statement {
    std::string name;
    uint16_t offset = 0;

    Label(const std::string& name) : name(name) {}

    StatementType GetType() override { return StatementType::Label; }

    void Print() override { std::cout << name << ":" << std::endl; }

    int GetOffset() override { return offset; }
};

class ParseException : public std::exception {
   public:
    explicit ParseException(const char* message) : msg(message) {}

    explicit ParseException(const std::string& message) : msg(message) {}

    virtual ~ParseException() throw() {}

    virtual const char* what() const throw() { return msg.c_str(); }

   protected:
    std::string msg;
};

class AST {
    std::list<std::unique_ptr<Statement>> list;

   public:
    using iterator = typename std::list<std::unique_ptr<Statement>>::iterator;
    using NodeType = std::unique_ptr<Statement>;

    AST() = default;
    AST(AST&& other) : list(std::move(other.list)) {}
    AST& operator=(AST&& other)
    {
        list = std::move(other.list);
        return *this;
    }

    iterator FindNodeByIndex(uint16_t index)
    {
        auto it = std::find_if(list.begin(), list.end(),
                               [index](const std::unique_ptr<Statement>& n) {
                                   return n->GetOffset() == index;
                               });

        if (it == list.end()) {
            std::stringstream ss;
            ss << "Node at index " << index << "doesn't exist";
            throw ParseException(ss.str());
        }

        return it;
    }

    iterator InsertBefore(uint16_t index, std::unique_ptr<Statement>&& stmt)
    {
        auto it = FindNodeByIndex(index);
        list.insert(it, std::move(stmt));
        return it;
    }

    iterator Insert(iterator it, std::unique_ptr<Statement>&& stmt)
    {
        return list.insert(it, std::move(stmt));
    }

    void Erase(iterator it)
    {
        if (it == list.end())
            throw ParseException("Can't erase: node doesn't exist");
        list.erase(it);
    }
    void Erase(iterator begin, iterator end)
    {
        if (begin == list.end())
            throw ParseException("Can't erase out of bounds iterators");
        list.erase(begin, end);
    }

    iterator begin() { return list.begin(); }
    iterator end() { return list.end(); }
};

class Disassembler {
    AST ast;
    std::vector<uint8_t> data;
    std::queue<AST::iterator> branches;
    size_t program_size;

    AST::iterator InsertLabelBefore(uint16_t index, const std::string& name)
    {
        auto label = llvmes::make_unique<Label>(name);
        label->offset = index;
        auto it = ast.InsertBefore(index, std::move(label));
        return it;
    }

    void ReplaceWithInstruction(AST::iterator it)
    {
        while (it != ast.end()) {
            // Already an instruction, mark the branch as complete
            if ((*it)->GetType() == StatementType::Instruction) {
                break;
            }
            // Don't replace labels
            else if ((*it)->GetType() == StatementType::Label) {
                it = std::next(it);
                continue;
            }

            // Get offset of current statement
            uint16_t offs = (*it)->GetOffset();
            // Get the opcode from the ROM
            uint8_t opcode = data[offs];

            // This contains information about the instruction
            auto instr_info = MOS6502::DecodeInstruction(opcode);
            auto new_instr = make_unique<Instruction>();

            new_instr->offset = offs;
            new_instr->name = "Instr";  // TODO: Add instruction names
            new_instr->size = MOS6502::InstructionSize(instr_info.addr_mode);
            new_instr->addressing_mode = instr_info.addr_mode;
            new_instr->op_type = instr_info.op;
            new_instr->opcode = opcode;

            // Get a pointer since it will be moved into the AST
            Instruction* instr = new_instr.get();
            *it = std::move(new_instr);

            // Delete the "argument databytes" and replace with instruction
            // e.g. if length is 3 then the 2 bytes that carries data will get
            // removed from the AST since they belong to the instruction

            assert(instr->size == 1 || instr->size == 2 || instr->size == 3);

            if (instr->size == 2) {
                if (offs + 1 >= data.size())
                    throw ParseException("Machine code has illegal format");

                ast.Erase(std::next(it));
                instr->arg = data[offs + 1];
            }
            else if (instr->size == 3) {
                if (offs + 2 >= data.size())
                    throw ParseException("Machine code has illegal format");

                ast.Erase(std::next(it, 1), std::next(it, 3));
                instr->arg = data[offs + 1] | data[offs + 2] << 8;
            }

            bool JMP_Abs = instr->opcode == 0x4C;
            if (IsBranch(instr_info.op) || JMP_Abs) {
                instr->is_branchinstruction = true;
                uint16_t target_index = 0;

                // Unconditional branch
                if (instr_info.op == MOS6502::Op::JSR || JMP_Abs) {
                    target_index = instr->arg;
                }
                // Conditional branch
                else {
                    target_index = (int8_t)instr->arg + offs + 2;
                }

                std::stringstream ss;
                ss << "Label " << ToHexString(target_index);

                auto branch_target = InsertLabelBefore(target_index, ss.str());
                if (branch_target == ast.end())
                    throw ParseException(
                        "Trying to insert a label before a node that doesn't "
                        "exist");
                branches.push(branch_target);

                instr->target_label = ss.str();

                // JMP ends a branch
                if (JMP_Abs)
                    break;
            }

            it = std::next(it);
        }
        branches.pop();
    }

   public:
    Disassembler(std::vector<uint8_t>&& data_in)
        : data(0x10000), program_size(data_in.size())
    {
        std::vector<uint8_t> temp = data_in;
        std::copy(temp.begin(), temp.end(), data.begin() + 0x8000);
    }

    std::vector<uint8_t> GetRAM() { return data; }

    AST&& Disassemble()
    {
        // Set all the statements to "data bytes"
        for (int i = 0x8000; i < (0x8000 + program_size); i++) {
            auto db = make_unique<DataByte>(data[i]);
            db->offset = i;
            ast.Insert(ast.end(), std::move(db));
        }

        // TODO: Figure out where the program will be in the address space
        // Will it be in 0x8000 -> 0xFFFF like the NES or in any place, if so
        // can we reserve some address for a custom ABI such as 'putchar'.
        // How will the system look? Amount of RAM etc, the program itself has
        // to be written for some system in mind?
        //
        // Hardcoded to start address 0x8000
        auto it = InsertLabelBefore(0x8000, "Reset");

        ReplaceWithInstruction(it);
        while (!branches.empty())
            ReplaceWithInstruction(branches.front());

        return std::move(ast);
    }
};
}  // namespace llvmes
