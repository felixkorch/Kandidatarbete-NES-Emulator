#include <imgui.h>
#include <llvmes/graphics/application.h>
#include <llvmes/graphics/log.h>
#include <llvmes/interpreter/cpu.h>

#include <filesystem>
#include <fstream>
#include <iostream>

#include "ext/imgui-filebrowser/imfilebrowser.h"
#include "ext/imgui_memory_editor/imgui_memory_editor.h"

#include "cache.h"
#include "imgui_log.h"

using namespace llvmes;

class Debugger : public gfx::Application {
    volatile bool cpu_should_run = false;
    bool open_memory_editor = false;

    std::uint8_t x = 0, y = 0, a = 0, sp = 0;
    std::uint16_t pc = 0;

    llvmes::CPU cpu;
    std::vector<char> memory;

    BasicLog log;
    llvmes::DisassemblyMap disassembly;

    ImGui::FileBrowser file_dialog;
    std::vector<std::string> cache;
    MemoryEditor mem_edit;

   public:
    Debugger() : gfx::Application(1200, 800, "LLVMES - Debugger"), memory(0x10000)
    {
        cpu.Read = [this](std::uint16_t addr) { return memory[addr]; };
        cpu.Write = [this](std::uint16_t addr, std::uint8_t data) {
            memory[addr] = data;
        };
        cpu.Reset();
        cpu.reg_pc = 0x0400;

        // Load the recently opened files
        cache = RecentlyOpened::GetCache();
    }

    void Stop() { cpu_should_run = false; }

    void Step()
    {
        cpu.Step();
        x = cpu.reg_x;
        y = cpu.reg_y;
        a = cpu.reg_a;
        sp = cpu.reg_sp;
        pc = cpu.reg_pc;

        auto entry = disassembly.find(pc);
        std::string str = (entry != disassembly.end()) ? entry->second : "Illegal OP";

        log.AddLog("[%s]\t %s\n", ToHexString(pc).c_str(),
                   str.c_str());
    }

    void Reset()
    {
        cpu.Reset();
        cpu.reg_pc = 0x0400;
        log.Clear();
        x = cpu.reg_x;
        y = cpu.reg_y;
        a = cpu.reg_a;
        sp = cpu.reg_sp;
        pc = cpu.reg_pc;
    }

    void RunCPU()
    {
        if (cpu_should_run)
            return;

        cpu_should_run = true;
        std::thread worker([this]() {
            while (cpu_should_run) {
                switch (cpu.reg_pc) {
                    case 0x336D:
                        LLVMES_TRACE("Skip decimal add/subtract test.");
                        cpu.reg_pc = 0x3405;
                        break;
                    case 0x3411:
                        LLVMES_TRACE("Skip decimal/binary switch test.");
                        cpu.reg_pc = 0x345D;
                        break;
                    case 0x346F:
                        LLVMES_TRACE("Skip decimal/binary switch test 2.");
                        cpu.reg_pc = 0x35A1;
                        break;
                    case 0x3469:
                        LLVMES_INFO("Test succeeded");
                        return;
                    default:
                        break;
                }

                cpu.Step();
                x = cpu.reg_x;
                y = cpu.reg_y;
                a = cpu.reg_a;
                sp = cpu.reg_sp;
                pc = cpu.reg_pc;
            }
            LLVMES_TRACE("Worker thread done");
        });
        worker.detach();
    }

    void OpenFile(const std::string& path)
    {
        LLVMES_TRACE("Attempting to load file: {}", path);
        std::ifstream in{path, std::ios::binary};
        if (in.fail())
            throw std::runtime_error(
                "Something went wrong with opening the file!");

        std::copy(std::istreambuf_iterator<char>(in),
                  std::istreambuf_iterator<char>(), memory.begin());

        disassembly = cpu.Disassemble(0x0000, 0xFFFF);
        RecentlyOpened::Write(path);
        LLVMES_INFO("Successfully loaded file");
    }

    void ShowFileDialog()
    {
        file_dialog.Display();

        if (file_dialog.HasSelected()) {
            try {
                OpenFile(file_dialog.GetSelected().string());
            }
            catch (std::runtime_error& e) {
                e.what();
            }

            file_dialog.ClearSelected();
        }
    }

    void ShowMenuBar()
    {
        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("Open", "Ctrl+O")) {
                    file_dialog.SetTypeFilters({".bin", ".asm"});
                    file_dialog.SetTitle("Open a binary file");
                    file_dialog.Open();
                }

                if (cache.empty()) {
                    ImGui::MenuItem("Open Recent");
                }
                else {
                    if (ImGui::BeginMenu("Open Recent")) {
                        for (const std::string& line : cache) {
                            fs::path p(line);
                            std::string file_name = p.filename().string();
                            if (ImGui::MenuItem(file_name.c_str()))
                                OpenFile(line);
                        }
                        ImGui::EndMenu();
                    }
                }
                if (ImGui::MenuItem("Quit", "Alt+F4")) {
                    Terminate();
                }

                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("View")) {
                if (ImGui::MenuItem("Memory Editor", "Ctrl+M")) {
                    open_memory_editor = !open_memory_editor;
                }
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }
    }

    void ShowRegisterView()
    {
        ImGui::SetNextWindowPos(ImVec2(50, 80));
        ImGui::SetNextWindowSize(ImVec2(400, 350));
        ImGui::Begin("Register View", nullptr,
                     ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
        ImGui::Text("Register View");

        ImGui::Columns(2, "outer", false);
        ImGui::Separator();
        {
            ImGui::Dummy(ImVec2(0.0f, 20.0f));
            ImGui::Text("Reg A: %s", ToHexString(a).c_str());
            ImGui::Dummy(ImVec2(0.0f, 10.0f));
            ImGui::Text("Reg X: %s", ToHexString(x).c_str());
            ImGui::Dummy(ImVec2(0.0f, 10.0f));
            ImGui::Text("Reg Y: %s", ToHexString(y).c_str());
            ImGui::Dummy(ImVec2(0.0f, 10.0f));
            ImGui::Text("Reg SP: %s", ToHexString(sp).c_str());
            ImGui::Dummy(ImVec2(0.0f, 10.0f));
            ImGui::Text("Reg PC: %s", ToHexString(pc).c_str());

            ImGui::NextColumn();

            ImGui::Dummy(ImVec2(0.0f, 20.0f));
            if (ImGui::Button("Step", ImVec2(120, 50)))
                Step();
            ImGui::Dummy(ImVec2(0.0f, 10.0f));
            if (ImGui::Button(cpu_should_run ? "Stop" : "Run",
                              ImVec2(120, 50))) {
                cpu_should_run ? Stop() : RunCPU();
            }
            ImGui::Dummy(ImVec2(0.0f, 10.0f));
            if (ImGui::Button("Reset", ImVec2(120, 50)))
                Reset();
        }

        ImGui::End();
    }

    void OnImGui() override
    {
        ShowFileDialog();
        ShowMenuBar();
        ShowRegisterView();
        log.Draw("Disassembly History");

        if (open_memory_editor)
            mem_edit.DrawWindow("Memory Editor", memory.data(), memory.size());
    }
};

int main()
try {
    Debugger ui;
    ui.Run();
}
catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
}