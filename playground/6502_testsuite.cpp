#include <fstream>
#include <iostream>
#include <string>

#include "llvmes/interpreter/cpu.h"

using namespace llvmes;

int main(int argc, char** argv)
try {
    std::ifstream in{"6502_functional_test.bin", std::ios::binary};
    if (in.fail())
        throw std::runtime_error("The file doesn't exist");
    auto program = std::vector<char>{std::istreambuf_iterator<char>(in),
                                     std::istreambuf_iterator<char>()};

    CPU cpu;
    cpu.Read = [program](std::uint16_t addr) { return program[addr]; };
    cpu.Write = [program](std::uint16_t addr, std::uint8_t data) mutable {
        return program[addr] = data;
    };
    cpu.Reset();

    cpu.Run();

    return 0;
}
catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
}
