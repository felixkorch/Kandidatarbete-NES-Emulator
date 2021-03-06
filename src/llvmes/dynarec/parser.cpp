#include "llvmes/dynarec/parser.h"

#include "llvmes/dynarec/6502_opcode.h"

namespace llvmes {
namespace dynarec {

Parser::Parser(std::vector<uint8_t>&& data_in, uint16_t start_location)
    : data(0x10000), program_size(data_in.size()), start_location(start_location)
{
    auto temp = std::move(data_in);
    if (start_location + program_size >= 0xFFFF)
        throw ParseException("Program doesn't fit in that space");
    std::copy(temp.begin(), temp.end(), data.begin() + start_location);
}

// TODO: Define 0x8D and/or 0x200F with names in some central place?
bool IsAbiReturn(Instruction* instruction)
{
    uint8_t op_code = instruction->opcode;
    uint16_t argument = instruction->arg;
    // According to our ABI, this is a return statement
    return op_code == 0x8D && argument == 0x200F;
}

// TODO: Add more checks? Evaluate if we need to distinguise between
bool IsJumpReturn(Instruction* instruction)
{
    // Absolute and Indirect
    return instruction->opcode == 0x4C;
}

bool IsJumpIndirect(Instruction* instruction)
{
    // Absolute and Indirect
    return instruction->opcode == 0x6C;
}

bool IsRTS(Instruction* instruction)
{
    return instruction->opcode == 0x60;
}

bool IsBranchInstruction(Instruction* instruction)
{
    return IsBranch(instruction->op_type) || IsJumpReturn(instruction) ||
           IsJumpIndirect(instruction);
}

uint16_t ParseBranchTarget(Instruction* instruction)
{
    uint16_t target;
    uint16_t argument = instruction->arg;
    uint16_t index = instruction->offset;

    if (instruction->op_type == MOS6502::Op::JSR || IsJumpReturn(instruction)) {
        target = argument;
    }
    else if (instruction->op_type == MOS6502::Op::JMP &&
             instruction->addressing_mode == MOS6502::AddressingMode::Indirect) {
        target = (uint16_t)instruction->offset + 3;
    }
    // Conditional branch
    else {
        target = (int8_t)argument + index + 2;
    }
    return target;
}

//  TODO: Maybe break this up a bit to increase readability
void Parser::ParseInstructions(uint16_t start)
{
    index = start;
    while (1) {
        // Already parsed
        if (instructions.count(index))
            break;

        uint8_t opcode = data[index];

        // This contains information about the instruction
        MOS6502::Instruction mos_instr = MOS6502::DecodeInstruction(opcode);
        Instruction* instr = new Instruction;

        instr->offset = index;
        instr->name = "Instr";  // TODO: Add instruction names
        instr->size = MOS6502::InstructionSize(mos_instr.addr_mode);
        instr->addressing_mode = mos_instr.addr_mode;
        instr->op_type = mos_instr.op;
        instr->opcode = opcode;
        instr->arg = ParseArgument(instr);

        instructions[index] = instr;

        if (IsAbiReturn(instr))
            break;

        if (IsRTS(instr))
            break;

        if (IsBranchInstruction(instr)) {
            instr->target_label = AddLabel(instr);
            // JMP ends a branch
            if (IsJumpReturn(instr) || IsJumpIndirect(instr))
                break;
        }

        index += instr->size;
    }
}

// Returns the label to be added. If label already exists, return name of that
// label.
Label Parser::AddLabel(Instruction* instruction)
{
    uint16_t target_index = ParseBranchTarget(instruction);
    bool label_exists = labels.count(target_index) > 0;

    if (label_exists) {
        return labels[target_index];
    }
    else {
        std::stringstream ss;
        ss << "Label " << ToHexString(target_index);
        branches.push(target_index);
        Label new_label = {target_index, ss.str()};

        labels[target_index] = new_label;
        return new_label;
    }
}

uint16_t Parser::ParseArgument(Instruction* instruction)
{
    assert(instruction->size == 1 || instruction->size == 2 || instruction->size == 3);

    uint16_t argument = 0;
    if (instruction->size == 2) {
        if (index + 1 >= data.size())
            throw ParseException("Machine code has illegal format");

        argument = data[index + 1];
    }
    else if (instruction->size == 3) {
        if (index + 2 >= data.size())
            throw ParseException("Machine code has illegal format");

        argument = data[index + 1] | data[index + 2] << 8;
    }

    return argument;
}

ParseResult Parser::Parse()
{
    uint16_t reset = data[0xFFFC] | (data[0xFFFD] << 8);
    reset_address = reset;
    labels[reset_address] = {reset_address, "Reset"};
    index = reset_address;
    branches.push(index);

    do {
        ParseInstructions(branches.front());
        branches.pop();
    } while (!branches.empty());

    return {labels, instructions, std::move(data)};
}
}  // namespace dynarec
}  // namespace llvmes
