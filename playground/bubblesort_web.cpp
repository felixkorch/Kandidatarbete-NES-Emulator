#include <imgui.h>
#include <llvmes-gui/application.h>
#include <llvmes-gui/draw.h>
#include <llvmes-gui/log.h>
#include <llvmes/common.h>
#include <llvmes/interpreter/cpu.h>
#include <emscripten/emscripten.h>

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
    std::vector<uint8_t> backup, memory;
    CPU cpu;
    volatile bool is_running = false;

   public:
    BubbleSort() : Application(800, 600, "BubbleSort"), memory(0x10000), backup(0x10000)
    {
        CreateCPU(PROGRAM_NAME);
    }

    void CreateCPU(const std::string& path)
    {
        std::ifstream in{path, std::ios::binary};
        if (in.fail())
            throw std::runtime_error("Place bubblesort.bin in this folder");

        std::copy(std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>(),
                  memory.begin() + 0x8000);

        backup = memory;

        cpu.Read = [this](uint16_t addr) { return memory[addr]; };
        cpu.Write = [this](uint16_t addr, uint8_t data) {
            memory[addr] = data;
            if (addr == 0x200F)
                is_running = false;
        };

        cpu.Reset();
    }

    void Go()
    {
        while (is_running) {
            cpu.Step();
        }
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
                pthread_t worker;
                int res = pthread_create(
                    &worker, NULL,
                    [](void* p) -> void* {
                        BubbleSort* c = (BubbleSort*)p;
                        c->Go();
                        LLVMES_TRACE("Program done executing.");
                        return nullptr;
                    },
                    this);
                LLVMES_ASSERT(res == 0, "Thread creation failed");
                pthread_detach(worker);
            }
            else if (ev.GetKeyCode() == LLVMES_KEY_R) {
                if (is_running)
                    return;
                memory = backup;
                cpu.Reset();
                LLVMES_TRACE("Reset!");
            }
        }
        else if (e.GetEventType() == EventType::DropEvent) {
            auto& drop_ev = (DropEvent&)e;
            CreateCPU(drop_ev.GetPath());
        }
    }

    void OnUpdate() override
    {
        Draw::Begin();
        for (int i = 0; i < ELEMENT_COUNT; i++) {
            uint16_t index = BASE_ADDR + i;
            uint8_t element = memory[index];
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