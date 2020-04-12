#include "llvmes/dynarec/compiler.h"

namespace llvmes {

// Only one compiler can exist atm
// This is a static ref to the compiler, a hack to keep the "read/write" functions static
static Compiler* s_compiler = nullptr;

void write_memory(int16_t addr, int8_t val)
{
    s_compiler->Write(addr, val);
}

int8_t read_memory(int16_t addr)
{
    return s_compiler->Read(addr);
}

inline void putreg(int8_t r)
{
    printf("%s\n", ToHexString(r).c_str());
}

inline void putchar(int8_t c)
{
    printf("%c", c);
}

Compiler::Compiler(AST&& ast, const std::string& program_name)
    : ast(std::move(ast)), c(llvmes::make_unique<Compilation>(program_name))
{
    assert(s_compiler == nullptr);
    s_compiler = this;

    int64 = llvm::Type::getInt64Ty(c->m->getContext());
    int32 = llvm::Type::getInt32Ty(c->m->getContext());
    int16 = llvm::Type::getInt16Ty(c->m->getContext());
    int8 = llvm::Type::getInt8Ty(c->m->getContext());
    int1 = llvm::Type::getInt1Ty(c->m->getContext());
    void_ty = llvm::Type::getVoidTy(c->m->getContext());

    // Declare putreg
    llvm::Type* putreg_result_type = void_ty;
    llvm::Type* putreg_argument_types[1] = {int8};
    bool putreg_vararg = false;

    llvm::FunctionType* putreg_function_type = llvm::FunctionType::get(
        putreg_result_type, putreg_argument_types, putreg_vararg);

    llvm::Function* putreg_function = llvm::Function::Create(
        putreg_function_type, llvm::Function::ExternalLinkage, "putreg", *c->m);

    c->putreg_fn = putreg_function;

    // Declare putchar
    llvm::Type* putchar_result_type = void_ty;
    llvm::Type* putchar_argument_types[1] = {int8};
    bool putchar_vararg = false;

    llvm::FunctionType* putchar_function_type = llvm::FunctionType::get(
        putchar_result_type, putchar_argument_types, putchar_vararg);

    llvm::Function* putchar_function = llvm::Function::Create(
        putchar_function_type, llvm::Function::ExternalLinkage, "putchar",
        *c->m);

    c->putchar_fn = putchar_function;

    // Main starts here

    llvm::Type* result_type = int32;
    llvm::Type* argument_types[2] = {};
    bool vararg = false;

    llvm::FunctionType* function_type =
        llvm::FunctionType::get(result_type, {}, vararg);

    llvm::Function* function = llvm::Function::Create(
        function_type, llvm::Function::ExternalLinkage, "main", *c->m);

    c->main_fn = function;

    llvm::BasicBlock* entry =
        llvm::BasicBlock::Create(c->m->getContext(), "entry", function);
    c->builder.SetInsertPoint(entry);

    c->reg_sp = c->builder.CreateAlloca(int16, 0, "SP");
    c->reg_x = c->builder.CreateAlloca(int8, 0, "X");
    c->reg_y = c->builder.CreateAlloca(int8, 0, "Y");
    c->reg_a = c->builder.CreateAlloca(int8, 0, "A");
    c->status_z = c->builder.CreateAlloca(int1, 0, "Z");
    c->status_n = c->builder.CreateAlloca(int1, 0, "N");
    c->status_v = c->builder.CreateAlloca(int1, 0, "V");
    c->status_c = c->builder.CreateAlloca(int1, 0, "C");

    // llvm::Type* array_ty = llvm::ArrayType::get(int8, 0xFFFF);
    // c->ram = c->builder.CreateAlloca(array_ty, nullptr, "ram");

    c->builder.CreateStore(GetConstant8(0), c->reg_x);
    c->builder.CreateStore(GetConstant8(0), c->reg_y);
    c->builder.CreateStore(GetConstant8(0), c->reg_a);

    // Write

    llvm::Type* w_result_type = void_ty;
    llvm::Type* w_argument_types[2] = {int16, int8};
    bool w_vararg = false;

    llvm::FunctionType* w_function_type =
        llvm::FunctionType::get(w_result_type, w_argument_types, w_vararg);

    llvm::Function* w_function = llvm::Function::Create(
        w_function_type, llvm::Function::ExternalLinkage, "write", *c->m);

    c->write_fn = w_function;

    // Read

    llvm::Type* r_result_type = int8;
    llvm::Type* r_argument_types[1] = {int16};
    bool r_vararg = false;

    llvm::FunctionType* r_function_type =
        llvm::FunctionType::get(r_result_type, r_argument_types, r_vararg);

    llvm::Function* r_function = llvm::Function::Create(
        r_function_type, llvm::Function::ExternalLinkage, "read", *c->m);

    c->read_fn = r_function;
}

std::function<int()> Compiler::GetMain(bool optimize)
{
    if (optimize)
        c->jitter.enable_optimize_module(true);
    c->jitter.enable_validate_module(true);

    auto ok = c->jitter.add_module(std::move(c->m));

    if (!ok)
        printf("Compilation failed!\n");
    auto fn_ptr = (int (*)())c->jitter.get_symbol_address("main");
    return std::function<int()>(fn_ptr);
}

void Compiler::PassOne()
{
    for (const auto& node : ast) {
        if (node->GetType() == StatementType::Label) {
            Label& l = (Label&)*node;
            llvm::BasicBlock* bb = llvm::BasicBlock::Create(
                c->m->getContext(), l.name, (llvm::Function*)c->main_fn);
            c->basicblocks[l.name] = bb;
        }
    }
}

void Compiler::PassTwo()
{
    c->builder.CreateBr(c->basicblocks["Reset"]);

    for (auto it = ast.begin(); it != ast.end(); ++it) {
        if ((*it)->GetType() == StatementType::Instruction) {
            Instruction& instr = (Instruction&)*(*it);
            CodeGen(instr);

            // TODO: Don't know if this is a one-case scenario or not
            auto next = std::next(it);
            if (next != ast.end()) {
                if ((*next)->GetType() == StatementType::Label &&
                    instr.is_branchinstruction == false) {
                    c->builder.CreateBr(
                        c->basicblocks[(*next)->GetAs<Label>().name]);
                }
            }
        }
        else if ((*it)->GetType() == StatementType::Label) {
            Label& l = (Label&)*(*it);
            c->builder.SetInsertPoint(c->basicblocks[l.name]);
        }
    }

    c->builder.CreateRet(GetConstant32(0));
}

void Compiler::Compile()
{
    PassOne();
    PassTwo();
}

}