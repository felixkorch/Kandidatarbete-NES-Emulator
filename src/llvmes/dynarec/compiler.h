#pragma once

#include "jitter/jitter.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/Support/TargetSelect.h"
#include "llvmes/dynarec/disassembler.h"

namespace llvmes {

void putreg(int8_t r);
void putchar(int8_t c);
void write_memory(int16_t addr, int8_t val);
int8_t read_memory(int16_t addr);

struct Compilation {
    JITTIR::Jitter jitter;
    std::unique_ptr<llvm::Module> m;
    llvm::IRBuilder<> builder;

    // llvm::Value* ram = nullptr;
    llvm::Value* reg_x = nullptr;
    llvm::Value* reg_y = nullptr;
    llvm::Value* reg_a = nullptr;
    llvm::Value* reg_sp = nullptr;
    llvm::Value* status_v = nullptr;
    llvm::Value* status_n = nullptr;
    llvm::Value* status_c = nullptr;
    llvm::Value* status_z = nullptr;
    llvm::Value* main_fn = nullptr;
    llvm::Value* putreg_fn = nullptr;
    llvm::Value* putchar_fn = nullptr;
    llvm::Value* write_fn = nullptr;
    llvm::Value* read_fn = nullptr;

    std::vector<uint8_t> ram;

    std::unordered_map<std::string, llvm::BasicBlock*> basicblocks;

    Compilation(const std::string& program_name)
        : jitter(),
          m(jitter.create_module(program_name)),
          builder(m->getContext()),
          ram(0x10000)
    {
        jitter.set_external_ir_dump_directory(".");
        jitter.add_external_symbol("putreg", &putreg);
        jitter.add_external_symbol("putchar", &putchar);
        jitter.add_external_symbol("write", &write_memory);
        jitter.add_external_symbol("read", &read_memory);
    }
};

class Compiler {
    AST ast;
    std::unique_ptr<Compilation> c;

    // Common used types and constants
    llvm::Type* int64 = nullptr;
    llvm::Type* int32 = nullptr;
    llvm::Type* int16 = nullptr;
    llvm::Type* int8 = nullptr;
    llvm::Type* int1 = nullptr;
    llvm::Type* void_ty = nullptr;

    int auto_labels = 0;

    void CodeGen(Instruction& i);

   public:
    Compiler(AST&& ast, const std::string& program_name);

    void SetRAM(std::vector<uint8_t>&& data) { c->ram = std::move(data); }

    llvm::Constant* GetConstant1(bool v)
    {
        return llvm::ConstantInt::get(int1, v);
    }
    llvm::Constant* GetConstant8(uint8_t v)
    {
        return llvm::ConstantInt::get(int8, v);
    }
    llvm::Constant* GetConstant16(uint16_t v)
    {
        return llvm::ConstantInt::get(int16, v);
    }
    llvm::Constant* GetConstant32(uint32_t v)
    {
        return llvm::ConstantInt::get(int32, v);
    }
    llvm::Constant* GetConstant64(uint64_t v)
    {
        return llvm::ConstantInt::get(int64, v);
    }

    void SetDumpDir(const std::string& path)
    {
        c->jitter.set_external_ir_dump_directory(path);
    }

    std::function<int()> GetMain(bool optimize);

    void StaticTestZ(int v)
    {
        llvm::Constant* z = llvm::ConstantInt::get(int1, v == 0);
        c->builder.CreateStore(z, c->status_z);
    }

    void StaticTestN(int v)
    {
        llvm::Constant* n = llvm::ConstantInt::get(int1, v & 0x80);
        c->builder.CreateStore(n, c->status_n);
    }

    void DynamicTestZ(llvm::Value* v)
    {
        llvm::Value* is_zero =
            c->builder.CreateICmpEQ(v, GetConstant8(0), "eq");
        c->builder.CreateStore(is_zero, c->status_z);
    }
    void DynamicTestN(llvm::Value* v)
    {
        llvm::Constant* c_0x80 = llvm::ConstantInt::get(int8, 0x80);
        llvm::Value* do_and = c->builder.CreateAnd(v, c_0x80);
        llvm::Value* is_negative = c->builder.CreateICmpEQ(do_and, c_0x80);
        c->builder.CreateStore(is_negative, c->status_n);
    }
    void DynamicTestCCmp(llvm::Value* v)
    {
        llvm::Constant* c_0x0100 = llvm::ConstantInt::get(int16, 0x0100);
        llvm::Value* lesThen = c->builder.CreateICmpULT(v, c_0x0100);
        c->builder.CreateStore(lesThen, c->status_c);
    }
    // Calculates the ram-address as a constant-expr
    llvm::Value* GetRAMPtr(uint16_t addr)
    {
        llvm::Constant* ram_ptr_value =
            llvm::ConstantInt::get(llvm::Type::getInt64Ty(c->m->getContext()),
                                   (int64_t)c->ram.data() + (int64_t)addr);

        return llvm::ConstantExpr::getIntToPtr(
            ram_ptr_value, llvm::PointerType::getUnqual(
                               llvm::Type::getInt8Ty(c->m->getContext())));
    }

    // Keeping it here for reference
    //

    //     llvm::Value* GetRAMPtr(llvm::Value* offset)
    //     {
    //         llvm::Constant* ram_ptr_value =
    //             llvm::ConstantInt::get(llvm::Type::getInt64Ty(c->m->getContext()),
    //                                    (int64_t)c->ram.data());
    //
    //         llvm::PointerType* ptr_ptr_ty =
    //         llvm::PointerType::getUnqual(llvm::Type::getInt8PtrTy(c->m->getContext()));
    //         llvm::Value* ptr_to_ptr =
    //         c->builder.CreateIntToPtr(ram_ptr_value, ptr_ptr_ty);
    //         llvm::Value* ptr = c->builder.CreateLoad(ptr_to_ptr);
    //         return c->builder.CreateGEP(ptr, {GetConstant8(0), offset});
    //     }

    // Can be called by LLVM on runtime
    void Write(uint16_t addr, uint8_t val) { c->ram[addr] = val; }
    uint16_t Read(uint16_t addr) { return c->ram[addr]; }

    // These two functions are used to write/read -
    // to addresses that are known on compile-time
    void WriteMemory(uint16_t addr, llvm::Value* v)
    {
        llvm::Value* ram_ptr = GetRAMPtr(addr);
        c->builder.CreateStore(v, ram_ptr);
    }

    llvm::Value* ReadMemory(uint16_t addr)
    {
        llvm::Value* ram_ptr = GetRAMPtr(addr);
        return c->builder.CreateLoad(ram_ptr);
    }

    void StackPush(llvm::Value* v)
    {
        llvm::Value* load_sp =
            c->builder.CreateLoad(c->reg_sp);  // load_sp <- reg_sp
        llvm::Constant* c_0x0100 = llvm::ConstantInt::get(int16, 0x0100);
        llvm::Constant* c1_16 = llvm::ConstantInt::get(int16, 1);
        llvm::Value* addr = c->builder.CreateOr(
            {load_sp, c_0x0100});  // addr <- load_sp or 0x0100
        load_sp =
            c->builder.CreateSub(load_sp, c1_16);    // load_sp <- load_sp - 1
        c->builder.CreateStore(load_sp, c->reg_sp);  // reg_sp <- load_sp
        // WriteMemory(addr, v);                        // [addr] <- v
    }

    void CreateCondBranch(llvm::Value* pred, llvm::BasicBlock* target)
    {
        std::stringstream auto_label;
        auto_label << "AutoLabel " << auto_labels++;
        llvm::BasicBlock* continue_block = llvm::BasicBlock::Create(
            c->m->getContext(), auto_label.str(), (llvm::Function*)c->main_fn);
        c->builder.CreateCondBr(pred, target, continue_block);
        c->builder.SetInsertPoint(continue_block);
    }

    void PassOne();
    void PassTwo();
    void Compile();
};

}  // namespace llvmes
