#include "llvmes/dynarec/compiler.h"
#include "llvmes/dynarec/disassembler.h"


#define PRINT_A 0x8D, 0x09, 0x20
#define PRINT_X 0x4C, 0x14, 0x00
#define LDY_IMM(V) 0xA0, V
#define INX 0xE8
#define DEY 0x88
#define STX_ZPG_Y(V) 0x96, V
#define REL(X) 0xFE X
#define BNE(V) 0xD0, REL(V)
#define LDA_ABS(B1, B2) 0xAD, B2, B1

std::vector<uint8_t> program1 {
    0xA0, 0x0A,        // LDY, # 0x0A
    0xE8,              // INX -- Begin
    0x88,              // DEY
    0x4C, 0x14, 0x00,  // Print X
    0xD0, 0xF9,        // BNE, Begin
};

std::vector<uint8_t> program2 {
    0xA0, 0x0A,        // LDY, # 0x0A
    0xE8,              // INX -- Begin
    0x88,              // DEY
    0xD0, 0xFC,        // BNE, Begin
    0x8E, 0x00, 0x00,  // STX, $0000
    0xAD, 0x00, 0x00,  // LDA, $0000
    0x8D, 0x09, 0x20,  // Print A - should print 10
};

std::vector<uint8_t> program3 {
    LDY_IMM(0x0A),
    INX,                   // Loop
    DEY,
    STX_ZPG_Y(0x00),       // Write X to address 9-0
    BNE(-4),               // Br -> Loop
    LDA_ABS(0x00, 0x00),
    PRINT_A,               // 10
    LDA_ABS(0x00, 0x01),
    PRINT_A,               // 9
    LDA_ABS(0x00, 0x02),
    PRINT_A,               // 8
    LDA_ABS(0x00, 0x03),
    PRINT_A,               // 7
};

using namespace llvmes;

int main()
{
    auto d = llvmes::make_unique<Disassembler>(std::move(program3));

    AST ast;
    std::vector<uint8_t> ram;
    try {
        ast = d->Disassemble();
        ram = d->GetRAM();
    }
    catch (ParseException& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    auto c = llvmes::make_unique<Compiler>(std::move(ast), "load_store");
    c->SetRAM(std::move(ram));
    c->Compile();

    bool optimized = true;
    c->GetMain(optimized)();

    return 0;
}
