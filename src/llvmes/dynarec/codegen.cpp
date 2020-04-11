#include "llvmes/dynarec/compiler.h"

namespace llvmes {

void Compiler::CodeGen(Instruction& i)
{
    switch (i.opcode) {
        case 0xD0: {  // BNE Immediate
            llvm::Value* load_z = c->builder.CreateLoad(c->status_z);
            llvm::Value* is_nonzero =
                c->builder.CreateICmpNE(load_z, GetConstant1(1), "ne");
            CreateCondBranch(is_nonzero, c->basicblocks[i.target_label]);
            break;
        }
        case 0xF0: {  // BEQ Immediate
            llvm::Value* load_z = c->builder.CreateLoad(c->status_z);
            llvm::Value* is_zero =
                c->builder.CreateICmpEQ(load_z, GetConstant1(1), "eq");
            CreateCondBranch(is_zero, c->basicblocks[i.target_label]);
            break;
        }
        case 0x30: {  // BMI Immediate
            break;
        }
        case 0x90: {  // BCC Immediate
            break;
        }
        case 0xB0: {  // BCS Immediate
            break;
        }
        case 0x10: {  // BPL Immediate
            break;
        }
        case 0x50: {  // BVC Immediate
            break;
        }
        case 0x70: {  // BVS Immediate
            break;
        }
        case 0xE8: {  // INX Implied
            llvm::Value* load_x = c->builder.CreateLoad(c->reg_x);
            llvm::Value* inx = c->builder.CreateAdd(load_x, GetConstant8(1));
            c->builder.CreateStore(inx, c->reg_x);
            DynamicTestZ(inx);
            DynamicTestN(inx);
            break;
        }
        case 0xC8: {  // INY Implied
            break;
        }
        case 0x88: {  // DEY Implied
            llvm::Value* load_y = c->builder.CreateLoad(c->reg_y);
            llvm::Value* dey = c->builder.CreateSub(load_y, GetConstant8(1));
            c->builder.CreateStore(dey, c->reg_y);
            DynamicTestZ(dey);
            DynamicTestN(dey);
            break;
        }
        case 0xCA: {  // DEX Implied
            break;
        }
        case 0xE6: {  // INC Zeropage
            break;
        }
        case 0xF6: {  // INC ZeropageX
            break;
        }
        case 0xEE: {  // INC Absolute
            break;
        }
        case 0xFE: {  // INC AbsoluteX
            break;
        }
        case 0x4C: {  // JMP Absolute
            c->builder.CreateBr(c->basicblocks[i.target_label]);
            break;
        }
        case 0x6C: {  // JMP Indirect
            break;
        }
        case 0x20: {  // JSR Absolute
            break;
        }
        case 0x24: {  // BIT Zeropage
            break;
        }
        case 0x2C: {  // BIT Absolute
            break;
        }
        case 0x00: {  // BRK Implied
            break;
        }
        case 0x69: {  // ADC Immediate
            break;
        }
        case 0xC9: {  // CMP Immediate
            // In data
            llvm::Value* operand = llvm::ConstantInt::get(int8, i.arg);
            // Get reg_a
            llvm::Value* reg_a = c->builder.CreateLoad(c->reg_a);
            // Compare
            llvm::Value* result = c->builder.CreateSub(reg_a, operand);
            // Flag Test
            DynamicTestZ(result);
            DynamicTestN(result);
            // Makes the result a 16 bit by adding 8 zeros needs to be 
            // 16 bit in DynamicTestCCmp.
            llvm::Value* target_addr_16 =
                c->builder.CreateZExt(result, int16);
            DynamicTestCCmp(result);
            break;
        }
        case 0xC5: {  // CMP Zeropage
            // Get reg_a
            llvm::Value* reg_a = c->builder.CreateLoad(c->reg_a);
            // Get in data and compare
            llvm::Value* result =
                c->builder.CreateSub(reg_a, ReadMemory(i.arg));
            // Flag Test
            DynamicTestZ(result);
            DynamicTestN(result);
            // Makes the result a 16 bit by adding 8 zeros needs to be
            // 16 bit in DynamicTestCCmp.
            llvm::Value* target_addr_16 = c->builder.CreateZExt(result, int16);
            DynamicTestCCmp(result);
            break;
        }
        case 0xD5: {  // CMP ZeropageX
            // In data
            llvm::Value* ram_ptr = GetRAMPtr(i.arg);
            // Get reg_a and reg_x
            llvm::Value* reg_a = c->builder.CreateLoad(c->reg_a);
            llvm::Value* reg_x = c->builder.CreateLoad(c->reg_x);
            // Add data and reg_x
            llvm::Value* target_addr = c->builder.CreateAdd(ram_ptr, reg_x);
            // Get value to compare with
            llvm::Value* load_ram = c->builder.CreateLoad(target_addr);
            // Compare
            llvm::Value* result = c->builder.CreateSub(reg_a, load_ram);
            // Flag Test
            DynamicTestZ(result);
            DynamicTestN(result);
            // Makes the result a 16 bit by adding 8 zeros needs to be
            // 16 bit in DynamicTestCCmp.
            llvm::Value* target_addr_16 = c->builder.CreateZExt(result, int16);
            DynamicTestCCmp(result);
            break;
        }
        case 0xCD: {  // CMP Absolute
            // In data
            llvm::Value* load_ram = ReadMemory(i.arg);
            // Get reg_a
            llvm::Value* reg_a = c->builder.CreateLoad(c->reg_a);
            // Compare
            llvm::Value* result = c->builder.CreateSub(reg_a, load_ram);
            // Flag test
            DynamicTestZ(result);
            DynamicTestN(result);
            // Makes the result a 16 bit by adding 8 zeros needs to be
            // 16 bit in DynamicTestCCmp.
            llvm::Value* target_addr_16 = c->builder.CreateZExt(result, int16);
            DynamicTestCCmp(result);
            break;
        }
        case 0xDD: {  // CMP AbsoluteX
            // In data
            llvm::Value* ram_ptr = GetRAMPtr(i.arg);
            // Get reg_x
            llvm::Value* reg_x = c->builder.CreateLoad(c->reg_x);
            // Add reg_x and ram_ptr
            llvm::Value* target_addr = c->builder.CreateAdd(ram_ptr, reg_x);
            // Get date from pointer
            llvm::Value* load_ram = c->builder.CreateLoad(target_addr);
            // Get reg_a
            llvm::Value* reg_a = c->builder.CreateLoad(c->reg_a);
            // Compare
            llvm::Value* result = c->builder.CreateSub(reg_a, load_ram);
            // Flag test
            DynamicTestZ(result);
            DynamicTestN(result);
            // Makes the result a 16 bit by adding 8 zeros needs to be
            // 16 bit in DynamicTestCCmp.
            llvm::Value* target_addr_16 = c->builder.CreateZExt(result, int16);
            DynamicTestCCmp(result);
            break;
        }
        case 0xD9: {  // CMP AbsoluteY
            // In data
            llvm::Value* ram_ptr = GetRAMPtr(i.arg);
            // Get reg_y
            llvm::Value* reg_y = c->builder.CreateLoad(c->reg_y);
            // Add reg_y and ram_ptr
            llvm::Value* target_addr = c->builder.CreateAdd(ram_ptr, reg_y);
            // Get date from pointer
            llvm::Value* load_ram = c->builder.CreateLoad(target_addr);
            // Get reg_a
            llvm::Value* reg_a = c->builder.CreateLoad(c->reg_a);
            // Compare
            llvm::Value* result = c->builder.CreateSub(reg_a, load_ram);
            // Flag test
            DynamicTestZ(result);
            DynamicTestN(result);
            // Makes the result a 16 bit by adding 8 zeros needs to be
            // 16 bit in DynamicTestCCmp.
            llvm::Value* target_addr_16 = c->builder.CreateZExt(result, int16);
            DynamicTestCCmp(result);
            break;
        }
        case 0xC1: {  // CMP IndirectX
            break;
        }
        case 0xD1: {  // CMP IndirectY
            break;
        }
        case 0xE0: {  // CPX Immediate
            // In data
            llvm::Value* operand = llvm::ConstantInt::get(int8, i.arg);
            // Get reg_x
            llvm::Value* reg_x = c->builder.CreateLoad(c->reg_x);
            // Compare
            llvm::Value* result = c->builder.CreateSub(reg_x, operand);
            // Flag test
            DynamicTestZ(result);
            DynamicTestN(result);
            // Makes the result a 16 bit by adding 8 zeros needs to be
            // 16 bit in DynamicTestCCmp.
            llvm::Value* target_addr_16 = c->builder.CreateZExt(result, int16);
            DynamicTestCCmp(result);
            break;
        }
        case 0xE4: {  // CPX Zeropage
            // Get reg_x
            llvm::Value* reg_x = c->builder.CreateLoad(c->reg_x);
            // Get in data and compare 
            llvm::Value* result =
                c->builder.CreateSub(reg_x, ReadMemory(i.arg));
            // Flag Test
            DynamicTestZ(result);
            DynamicTestN(result);
            // Makes the result a 16 bit by adding 8 zeros needs to be
            // 16 bit in DynamicTestCCmp.
            llvm::Value* target_addr_16 = c->builder.CreateZExt(result, int16);
            DynamicTestCCmp(result);
            break;
        }
        case 0xEC: {  // CPX Absolute
            // Get reg_x
            llvm::Value* reg_x = c->builder.CreateLoad(c->reg_x);
            // Get in datad adn compare
            llvm::Value* result = c->builder.CreateSub(reg_x, ReadMemory(i.arg));
            // Flag test
            DynamicTestZ(result);
            DynamicTestN(result);
            // Makes the result a 16 bit by adding 8 zeros needs to be
            // 16 bit in DynamicTestCCmp.
            llvm::Value* target_addr_16 = c->builder.CreateZExt(result, int16);
            DynamicTestCCmp(result);
            break;
        }
        case 0xC0: {  // CPY Immediate
            // In data
            llvm::Value* operand = llvm::ConstantInt::get(int8, i.arg);
            // Get reg_y
            llvm::Value* reg_y = c->builder.CreateLoad(c->reg_y);
            // Compare
            llvm::Value* result = c->builder.CreateSub(reg_y, operand);
            // Flag test
            DynamicTestZ(result);
            DynamicTestN(result);
            // Makes the result a 16 bit by adding 8 zeros needs to be
            // 16 bit in DynamicTestCCmp.
            llvm::Value* target_addr_16 = c->builder.CreateZExt(result, int16);
            DynamicTestCCmp(result);
            break;
        }
        case 0xC4: {  // CPY Zeropage
            // Get reg_y
            llvm::Value* reg_y = c->builder.CreateLoad(c->reg_y);
            // Get in data and compare
            llvm::Value* result =
                c->builder.CreateSub(reg_y, ReadMemory(i.arg));
            // Flag Test
            DynamicTestZ(result);
            DynamicTestN(result);
            // Makes the result a 16 bit by adding 8 zeros needs to be
            // 16 bit in DynamicTestCCmp.
            llvm::Value* target_addr_16 = c->builder.CreateZExt(result, int16);
            DynamicTestCCmp(result);
            break;
        }
        case 0xCC: {  // CPY Absolute
            // Get reg_y
            llvm::Value* reg_y = c->builder.CreateLoad(c->reg_y);
            // Get in data and compare
            llvm::Value* result =
                c->builder.CreateSub(reg_y, ReadMemory(i.arg));
            // Flag test
            DynamicTestZ(result);
            DynamicTestN(result);
            // Makes the result a 16 bit by adding 8 zeros needs to be
            // 16 bit in DynamicTestCCmp.
            llvm::Value* target_addr_16 = c->builder.CreateZExt(result, int16);
            DynamicTestCCmp(result);
            break;
        }
        case 0xC6: {  // DEC Zeropage
            break;
        }
        case 0xD6: {  // DEC ZeropageX
            break;
        }
        case 0xCE: {  // DEC Absolute
            break;
        }
        case 0xDE: {  // DEC AbsoluteX
            break;
        }
        case 0x49: {  // EOR Immediate
            break;
        }
        case 0x45: {  // EOR Zeropage
            break;
        }
        case 0x55: {  // EOR ZeropageX
            break;
        }
        case 0x4D: {  // EOR Absolute
            break;
        }
        case 0x5D: {  // EOR AbsoluteX
            break;
        }
        case 0x59: {  // EOR AbsoluteY
            break;
        }
        case 0x41: {  // EOR IndirectX
            break;
        }
        case 0x51: {  // EOR IndirectY
            break;
        }
        case 0xA9: {  // LDA Immediate
            int arg = i.arg;
            llvm::Value* a = llvm::ConstantInt::get(int8, arg);
            c->builder.CreateStore(a, c->reg_a);
            StaticTestZ(arg);
            StaticTestN(arg);
            break;
        }
        case 0xA5: {  // LDA Zeropage
            llvm::Value* ram_ptr = GetRAMPtr(i.arg);
            llvm::Value* load_ram = c->builder.CreateLoad(ram_ptr);
            c->builder.CreateStore(load_ram, c->reg_a);
            break;
        }
        case 0xB5: {  // LDA ZeropageX
            llvm::Value* ram_ptr = GetRAMPtr(i.arg);
            // Loads the X register into a placeholder
            llvm::Value* load_x = c->builder.CreateLoad(c->reg_x);
            // Adds the X register to the RAM pointer
            llvm::Value* index_16 = c->builder.CreateAdd(ram_ptr, load_x);
            // AND with 0xFF to make sure that the index is 2 byte
            llvm::Value* zero_page_index = c->builder.CreateAnd(index_16, 0xFF);
            llvm::Value* value = c->builder.CreateLoad(zero_page_index);
            break;
        }
        case 0xA1: {  // LDA IndirectX
            break;
        }
        case 0xB1: {  // LDA IndirectY
            break;
        }

            // case 0xAD: { // LDA Absolute
            //     llvm::Value* ram_ptr = GetRAMPtr(i.arg);
            //     llvm::Value* load_ram = c->builder.CreateLoad(ram_ptr);
            //     c->builder.CreateStore(load_ram, c->reg_a);
            //     break;
            // }
        case 0xAD: {  // LDA Absolute
            c->builder.CreateStore(ReadMemory(i.arg), c->reg_a);
            break;
        }
        case 0xBD: {  // LDA AbsoluteX
            llvm::Value* ram_ptr = GetRAMPtr(i.arg);
            // Loads the X register into a placeholder
            llvm::Value* load_x = c->builder.CreateLoad(c->reg_x);
            // Adds the X register to the RAM pointer
            llvm::Value* index_16 = c->builder.CreateAdd(ram_ptr, load_x);
            // AND with 0xFFF to make sure that the index is 3 byte
            llvm::Value* zero_page_index =
                c->builder.CreateAnd(index_16, 0xFFF);
            llvm::Value* value = c->builder.CreateLoad(zero_page_index);
            break;
        }
        case 0xB9: {  // LDA AbsoluteY
            llvm::Value* ram_ptr = GetRAMPtr(i.arg);
            // Loads the Y register into a placeholder
            llvm::Value* load_y = c->builder.CreateLoad(c->reg_y);
            // Adds the Y register to the RAM pointer
            llvm::Value* index_16 = c->builder.CreateAdd(ram_ptr, load_y);
            // AND with 0xFFF to make sure that the index is 3 byte
            llvm::Value* zero_page_index =
                c->builder.CreateAnd(index_16, 0xFFF);
            llvm::Value* value = c->builder.CreateLoad(zero_page_index);
            break;
        }
        case 0xA2: {  // LDX Immediate
            int arg = i.arg;
            llvm::Value* x = llvm::ConstantInt::get(int8, arg);
            c->builder.CreateStore(x, c->reg_x);
            StaticTestZ(arg);
            StaticTestN(arg);
            break;
        }
        case 0xA6: {  // LDX Zeropage
            llvm::Value* ram_ptr = GetRAMPtr(i.arg);
            llvm::Value* load_ram = c->builder.CreateLoad(ram_ptr);
            c->builder.CreateStore(load_ram, c->reg_x);
            break;
        }
        case 0xB6: {  // LDX ZeropageY
            llvm::Value* ram_ptr = GetRAMPtr(i.arg);
            // Loads the Y register into a placeholder
            llvm::Value* load_y = c->builder.CreateLoad(c->reg_y);
            // Adds the Y register to the RAM pointer
            llvm::Value* index_16 = c->builder.CreateAdd(ram_ptr, load_y);
            // AND with 0xFF to make sure that the index is 2 byte
            llvm::Value* zero_page_index = c->builder.CreateAnd(index_16, 0xFF);
            llvm::Value* value = c->builder.CreateLoad(zero_page_index);
            break;
        }
        case 0xAE: {  // LDX Absolute
            llvm::Value* ram_ptr = GetRAMPtr(i.arg);
            llvm::Value* load_ram = c->builder.CreateLoad(ram_ptr);
            c->builder.CreateStore(load_ram, c->reg_x);
            break;
        }
        case 0xBE: {  // LDX AbsoluteY
            llvm::Value* ram_ptr = GetRAMPtr(i.arg);
            // Loads the Y register into a placeholder
            llvm::Value* load_y = c->builder.CreateLoad(c->reg_y);
            // Adds the Y register to the RAM pointer
            llvm::Value* index_16 = c->builder.CreateAdd(ram_ptr, load_y);
            // AND with 0xFFF to make sure that the index is 3 byte
            llvm::Value* zero_page_index =
                c->builder.CreateAnd(index_16, 0xFFF);
            llvm::Value* value = c->builder.CreateLoad(zero_page_index);
            break;
        }
        case 0xA0: {  // LDY Immediate
            uint8_t arg = i.arg;
            c->builder.CreateStore(GetConstant8(arg), c->reg_y);
            StaticTestZ(arg);
            StaticTestN(arg);
            break;
        }
        case 0xA4: {  // LDY Zeropage
            llvm::Value* ram_ptr = GetRAMPtr(i.arg);
            llvm::Value* load_ram = c->builder.CreateLoad(ram_ptr);
            c->builder.CreateStore(load_ram, c->reg_y);
            break;
        }
        case 0xB4: {  // LDY ZeropageX
            llvm::Value* ram_ptr = GetRAMPtr(i.arg);
            // Loads the X register into a placeholder
            llvm::Value* load_x = c->builder.CreateLoad(c->reg_x);
            // Adds the X register to the RAM pointer
            llvm::Value* index_16 = c->builder.CreateAdd(ram_ptr, load_x);
            // AND with 0xFF to make sure that the index is 2 byte
            llvm::Value* zero_page_index = c->builder.CreateAnd(index_16, 0xFF);
            llvm::Value* value = c->builder.CreateLoad(zero_page_index);
            break;
        }
        case 0xAC: {  // LDY Absolute
            llvm::Value* ram_ptr = GetRAMPtr(i.arg);
            llvm::Value* load_ram = c->builder.CreateLoad(ram_ptr);
            c->builder.CreateStore(load_ram, c->reg_y);
            break;
        }
        case 0xBC: {  // LDY AbsoluteX
            llvm::Value* ram_ptr = GetRAMPtr(i.arg);
            // Loads the X register into a placeholder
            llvm::Value* load_x = c->builder.CreateLoad(c->reg_x);
            // Adds the X register to the RAM pointer
            llvm::Value* index_16 = c->builder.CreateAdd(ram_ptr, load_x);
            // AND with 0xFFF to make sure that the index is 3 byte
            llvm::Value* zero_page_index =
                c->builder.CreateAnd(index_16, 0xFFF);
            llvm::Value* value = c->builder.CreateLoad(zero_page_index);
            break;
        }
        case 0x4A: {  // ACC Accumulator
            break;
        }
        case 0x46: {  // LSR Zeropage
            break;
        }
        case 0x56: {  // LSR ZeropageX
            break;
        }
        case 0x4E: {  // LSR Absolute
            break;
        }
        case 0x5E: {  // LSR AbsoluteX
            break;
        }
        case 0x09: {  // ORA Immediate
            break;
        }
        case 0x05: {  // ORA Zeropage
            break;
        }
        case 0x15: {  // ORA ZeropageX
            break;
        }
        case 0x0D: {  // ORA Absolute
            break;
        }
        case 0x1D: {  // ORA AbsoluteX
            break;
        }
        case 0x19: {  // ORA AbsoluteY
            break;
        }
        case 0x01: {  // ORA IndirectX
            break;
        }
        case 0x11: {  // ORA IndirectY
            break;
        }
        case 0x48: {  // PHA Implied
            break;
        }
        case 0x08: {  // PHP Implied
            break;
        }
        case 0x68: {  // PLA Implied
            break;
        }
        case 0x28: {  // PLP Implied
            break;
        }
        case 0x2A: {  // ACC Accumulator
            break;
        }
        case 0x26: {  // ROL Zeropage
            break;
        }
        case 0x36: {  // ROL ZeropageX
            break;
        }
        case 0x2E: {  // ROL Absolute
            break;
        }
        case 0x3E: {  // ROL AbsoluteX
            break;
        }
        case 0x6A: {  // ACC Accumulator
            break;
        }
        case 0x66: {  // ROR Zeropage
            break;
        }
        case 0x76: {  // ROR ZeropageX
            break;
        }
        case 0x6E: {  // ROR Absolute
            break;
        }
        case 0x7E: {  // ROR AbsoluteX
            break;
        }
        case 0x40: {  // RTI Implied
            break;
        }
        case 0x60: {  // RTS Implied
            break;
        }
        case 0xE9: {  // SBC Immediate
            break;
        }
        case 0xE5: {  // SBC Zeropage
            break;
        }
        case 0xF5: {  // SBC ZeropageX
            break;
        }
        case 0xED: {  // SBC Absolute
            break;
        }
        case 0xFD: {  // SBC AbsoluteX
            break;
        }
        case 0xF9: {  // SBC AbsoluteY
            break;
        }
        case 0xE1: {  // SBC IndirectX
            break;
        }
        case 0xF1: {  // SBC IndirectY
            break;
        }
        case 0x38: {  // SEC Implied
            break;
        }
        case 0xF8: {  // SED Implied
            break;
        }
        case 0x78: {  // SEI Implied
            break;
        }
        case 0x18: {  // CLC Implied
            break;
        }
        case 0xD8: {  // CLD Implied
            break;
        }
        case 0x58: {  // CLI Implied
            break;
        }
        case 0xB8: {  // CLV Implied
            break;
        }
        case 0x85: {  // STA Zeropage
            break;
        }
        case 0x95: {  // STA ZeropageX
            break;
        }
        case 0x8D: {  // STA Absolute
            uint16_t addr = i.arg;
            // Write to '0x2008' and 'A' will be written to stdout as char
            if (addr == 0x2008) {
                llvm::Value* load_a = c->builder.CreateLoad(c->reg_a);
                c->builder.CreateCall(c->putchar_fn, {load_a});
            }
            // Write A to stdout
            else if (addr == 0x2009) {
                llvm::Value* load_a = c->builder.CreateLoad(c->reg_a);
                c->builder.CreateCall(c->putreg_fn, {load_a});
            }
            // Write X to stdout
            else if (addr == 0x200A) {
                llvm::Value* load_x = c->builder.CreateLoad(c->reg_x);
                c->builder.CreateCall(c->putreg_fn, {load_x});
            }
            // Write Y to stdout
            else if (addr == 0x200B) {
                llvm::Value* load_y = c->builder.CreateLoad(c->reg_y);
                c->builder.CreateCall(c->putreg_fn, {load_y});
            }
            else if (addr == 0x200C) {
                llvm::Value* load_n = c->builder.CreateLoad(c->status_n);
                llvm::Value* result = c->builder.CreateZExt(load_n, int8);
                c->builder.CreateCall(c->putreg_fn, {result});
            }
            else if (addr == 0x200E) {
                llvm::Value* load_z = c->builder.CreateLoad(c->status_z);
                llvm::Value* result = c->builder.CreateZExt(load_z, int8);
                c->builder.CreateCall(c->putreg_fn, {result});
            }
            else {
                llvm::Value* load_a = c->builder.CreateLoad(c->reg_a);
                WriteMemory(addr, load_a);
            }
            break;
        }
        case 0x9D: {  // STA AbsoluteX
            break;
        }
        case 0x99: {  // STA AbsoluteY
            break;
        }
        case 0x81: {  // STA IndirectX
            break;
        }
        case 0x91: {  // STA IndirectY
            break;
        }
        case 0x86: {  // STX Zeropage
            llvm::Value* ram_ptr = GetRAMPtr(i.arg);
            llvm::Value* load_x = c->builder.CreateLoad(c->reg_x);
            c->builder.CreateStore(load_x, ram_ptr);
            break;
        }
        case 0x96: {  // STX ZeropageY
            llvm::Value* load_y = c->builder.CreateLoad(c->reg_y);
            llvm::Value* load_x = c->builder.CreateLoad(c->reg_x);
            llvm::Constant* zpg_addr = GetConstant8(i.arg);
            llvm::Value* target_addr = c->builder.CreateAdd(load_y, zpg_addr);
            llvm::Value* target_addr_16 =
                c->builder.CreateZExt(target_addr, int16);
            c->builder.CreateCall(c->write_fn, {target_addr_16, load_x});
            break;
        }
        case 0x8E: {  // STX Absolute
            llvm::Value* load_x = c->builder.CreateLoad(c->reg_x);
            WriteMemory(i.arg, load_x);
            break;
        }
        case 0x84: {  // STY Zeropage
            llvm::Value* load_y = c->builder.CreateLoad(c->reg_y);
            WriteMemory(i.arg, load_y);
            break;
        }
        case 0x94: {  // STY ZeropageX
            llvm::Value* X = c->builder.CreateLoad(c->reg_x);
            llvm::Value* ram_ptr = GetRAMPtr(i.arg);
            llvm::Value* sty_X = c->builder.CreateAdd(ram_ptr, X);
            llvm::Value* load_y = c->builder.CreateLoad(c->reg_y);
            c->builder.CreateStore(load_y, sty_X);
            break;
        }
        case 0x8C: {  // STY Absolute
            llvm::Value* ram_ptr = GetRAMPtr(i.arg);
            llvm::Value* load_y = c->builder.CreateLoad(c->reg_y);
            c->builder.CreateStore(load_y, ram_ptr);
            break;
        }
        case 0xAA: {  // TAX Implied

            llvm::Value* load_a = c->builder.CreateLoad(c->reg_a);
            llvm::Value* result = c->builder.CreateStore(load_a, c->reg_x);

            // flag test
            DynamicTestZ(result);
            DynamicTestN(result);
            break;
        }
        case 0xA8: {  // TAY Implied

            llvm::Value* load_a = c->builder.CreateLoad(c->reg_a);
            llvm::Value* result = c->builder.CreateStore(load_a, c->reg_y);

            // flag test
            DynamicTestZ(result);
            DynamicTestN(result);
            break;
        }
        case 0xBA: {  // TSX Implied
            llvm::Value* load_sp = c->builder.CreateLoad(c->reg_sp);
            llvm::Value* result = c->builder.CreateStore(load_sp, c->reg_x);

            // Flag test
            DynamicTestZ(result);
            DynamicTestN(result);
            break;
        }
        case 0x8A: {  // TXA Implied
            llvm::Value* load_x = c->builder.CreateLoad(c->reg_x);
            llvm::Value* result = c->builder.CreateStore(load_x, c->reg_a);

            // Flag test
            DynamicTestZ(result);
            DynamicTestN(result);
            break;
        }
        case 0x9A: {  // TXS Implied
            llvm::Value* load_x = c->builder.CreateLoad(c->reg_x);
            llvm::Value* result = c->builder.CreateStore(load_x, c->reg_sp);
            break;
        }
        case 0x98: {  // TYA Implied
            llvm::Value* load_y = c->builder.CreateLoad(c->reg_y);
            c->builder.CreateStore(load_y, c->reg_a);

            // Flag test
            DynamicTestN(load_y);
            DynamicTestZ(load_y);
            break;
        }
        case 0x29: {  // AND Immediate
            break;
        }
        case 0x25: {  // AND Zeropage
            break;
        }
        case 0x35: {  // AND ZeropageX
            break;
        }
        case 0x2D: {  // AND Absolute
            break;
        }
        case 0x3D: {  // AND AbsoluteX
            break;
        }
        case 0x39: {  // AND AbsoluteY
            break;
        }
        case 0x21: {  // AND IndirectX
            break;
        }
        case 0x31: {  // AND IndirectY
            break;
        }
        case 0x0A: {  // ACC Accumulator
            break;
        }
        case 0x06: {  // ASL Zeropage
            break;
        }
        case 0x16: {  // ASL ZeropageX
            break;
        }
        case 0x0E: {  // ASL Absolute
            break;
        }
        case 0x1E: {  // ASL AbsoluteX
            break;
        }
        case 0x61: {  // ADC IndirectX
            break;
        }
        case 0x71: {  // ADC IndirectY
            break;
        }
        case 0x65: {  // ADC Zeropage
            break;
        }
        case 0x75: {  // ADC ZeropageX
            break;
        }
        case 0x6D: {  // ADC Absolute
            break;
        }
        case 0x7D: {  // ADC AbsoluteX
            break;
        }
        case 0x79: {  // ADC AbsoluteY
            break;
        }
        case 0xEA: {  // NOP Implied
            break;
        }
    }
}
}  // namespace llvmes
