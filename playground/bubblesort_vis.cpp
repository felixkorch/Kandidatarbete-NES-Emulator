#include <imgui.h>
#include <llvmes-gui/application.h>
#include <llvmes-gui/draw.h>
#include <llvmes-gui/log.h>
#include <llvmes/common.h>
#include <llvmes/dynarec/compiler.h>
#include <llvmes/dynarec/parser.h>
#include <llvmes/interpreter/cpu.h>

#include <fstream>
#include <iostream>

using namespace llvmes;
using namespace gui;

#define PROGRAM_NAME "bubblesort.bin"
static constexpr size_t BASE_ADDR = 0x8000;
static constexpr size_t ELEMENT_COUNT = 8192;
static constexpr size_t WIDTH = 800;
static constexpr size_t HEIGHT = 600;

float ToFloatColor(uint8_t b)
{
    float b_f = b;
    const float mod = 255;
    return b_f / mod;
}

class BubbleSort : public Application {
    std::vector<uint8_t> backup;
    std::unique_ptr<CPU> cpu;
    std::unique_ptr<dynarec::Compiler> compiler;
    std::function<int(void)> main;
    bool use_jit = false;
    volatile bool is_running = false;
    std::thread t;

   public:
    BubbleSort() : Application(800, 600, "BubbleSort"), backup(0x10000)
    {
        std::ifstream in{PROGRAM_NAME, std::ios::binary};
        if (in.fail())
            throw std::runtime_error("Place bubblesort.bin in this folder");
        auto program = std::vector<uint8_t>{std::istreambuf_iterator<char>(in),
                                            std::istreambuf_iterator<char>()};

        // JIT Setup
        dynarec::Parser parser(std::move(program), BASE_ADDR);
        dynarec::ParseResult result;
        result = parser.Parse();
        compiler = std::make_unique<dynarec::Compiler>(result, PROGRAM_NAME);
        main = compiler->Compile(true);
        backup = compiler->GetMemory();

        // Interpreter Setup
        cpu = std::make_unique<CPU>();
        cpu->Read = [this](uint16_t addr) { return compiler->GetMemory()[addr]; };
        cpu->Write = [this](uint16_t addr, uint8_t data) {
            compiler->GetMemory()[addr] = data;
            if (addr == 0x200F)
                cpu->Halt();
        };

        cpu->Reset();
    }

    void OnImGui() override {}

    void OnEvent(Event& e) override
    {
        if (e.GetEventType() == EventType::KeyPressEvent) {
            auto& ev = (KeyPressEvent&)e;

            if (ev.GetKeyCode() == LLVMES_KEY_SPACE) {
                if (is_running == true)
                    return;
                is_running = true;
                if (use_jit) {
                    t = std::thread([this]() {
                        main();
                        is_running = false;
                        LLVMES_TRACE("Program done executing.");
                    });
                }
                else {
                    t = std::thread([this]() {
                        cpu->Run();
                        is_running = false;
                        LLVMES_TRACE("Program done executing.");
                    });
                }
                t.detach();
            }
            else if (ev.GetKeyCode() == LLVMES_KEY_I) {
                use_jit = false;
                LLVMES_INFO("Interpreter Mode selected");
            }
            else if (ev.GetKeyCode() == LLVMES_KEY_J) {
                use_jit = true;
                LLVMES_INFO("JIT Mode selected");
            }
            else if (ev.GetKeyCode() == LLVMES_KEY_R) {
                if (is_running)
                    return;
                compiler->GetMemory() = backup;
                cpu->Reset();
                LLVMES_TRACE("Reset!");
            }
        }
    }

    void OnUpdate() override
    {
        Draw::Begin();
        for (int i = 0; i < ELEMENT_COUNT; i++) {
            uint16_t index = BASE_ADDR + i;
            uint8_t element = compiler->GetMemory()[index];
            Draw::UseColor(0, ToFloatColor(element), ToFloatColor(element), 1);
            Draw::Rectangle((i * 8) % WIDTH, i / (WIDTH / 8) * 8, 8, 8);
        }
        Draw::End();
    }
};

int main()
try {
    BubbleSort ui;
    ui.Run();
}
catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
}