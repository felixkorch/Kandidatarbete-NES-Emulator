#include <imgui.h>
#include <llvmes/graphics/application.h>
#include <llvmes/graphics/log.h>
#include <llvmes/interpreter/cpu.h>

#include <fstream>
#include <iostream>

#include "ext/imgui-filebrowser/imfilebrowser.h"
#include "imgui_log.h"

using namespace llvmes;

class DebuggerGUI : public gfx::Application {
    bool p_open = true;

    std::uint8_t x = 0, y = 0, a = 0, sp = 0;
    std::uint16_t pc = 0;

    llvmes::CPU cpu;
    std::vector<char> memory;

    BasicLog log;
    llvmes::DisassemblyMap disassembly;

    ImGui::FileBrowser file_dialog;

   public:
    DebuggerGUI() : Application(1200, 800, "Test UI"), memory(0x10000)
    {
        cpu.Read = [this](std::uint16_t addr) { return memory[addr]; };
        cpu.Write = [this](std::uint16_t addr, std::uint8_t data) {
            memory[addr] = data;
        };
        cpu.Reset();
        cpu.reg_pc = 0x0400;
    }

    void Step()
    {
        cpu.Step();
        x = cpu.reg_x;
        y = cpu.reg_y;
        a = cpu.reg_a;
        sp = cpu.reg_sp;
        pc = cpu.reg_pc;

        auto entry = disassembly.find(pc);

        log.AddLog("[%s]:\t\t %s\n", ToHexString(entry->first).c_str(),
                   entry->second.c_str());
    }

    void OpenFile(const std::string& path)
    {
        std::ifstream in{path, std::ios::binary};
        if (in.fail())
            throw std::runtime_error(
                "Something went wrong with opening the file!");

        std::copy(std::istreambuf_iterator<char>(in),
                  std::istreambuf_iterator<char>(), memory.begin());

        disassembly = cpu.Disassemble(0x0000, 0xFFFF);
    }

    void OnImGui() override
    {
        file_dialog.Display();

        if (file_dialog.HasSelected()) {
            LLVMES_TRACE("[File] {}", file_dialog.GetSelected().string());
            try {
                OpenFile(file_dialog.GetSelected().string());
            }
            catch (std::runtime_error& e) {
                e.what();
            }

            file_dialog.ClearSelected();
        }

        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("Open", "Ctrl+O")) {
                    file_dialog.SetTypeFilters({".bin", ".asm"});
                    file_dialog.SetTitle("Open a binary file");
                    file_dialog.Open();
                }
                if (ImGui::BeginMenu("Open Recent")) {
                    ImGui::MenuItem("fish_hat.c");
                    ImGui::MenuItem("fish_hat.inl");
                    ImGui::MenuItem("fish_hat.h");
                    ImGui::EndMenu();
                }
                if (ImGui::MenuItem("Quit", "Alt+F4")) {
                }

                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }

        ImGui::SetNextWindowPos(ImVec2(50, 50));
        ImGui::SetNextWindowSize(ImVec2(400, 300));
        ImGui::Begin("Register View", &p_open,
                     ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
        ImGui::Text("Register View");

        ImGui::Columns(2, "outer", false);
        ImGui::Separator();
        {
            ImGui::Spacing();
            ImGui::Spacing();
            ImGui::Spacing();
            ImGui::Spacing();
            ImGui::Text("Reg A: %s", ToHexString(a).c_str());
            ImGui::Spacing();
            ImGui::Spacing();
            ImGui::Text("Reg X: %s", ToHexString(x).c_str());
            ImGui::Spacing();
            ImGui::Spacing();
            ImGui::Text("Reg Y: %s", ToHexString(y).c_str());
            ImGui::Spacing();
            ImGui::Spacing();
            ImGui::Text("Reg SP: %s", ToHexString(sp).c_str());
            ImGui::Spacing();
            ImGui::Spacing();
            ImGui::Text("Reg PC: %s", ToHexString(pc).c_str());

            ImGui::NextColumn();

            ImGui::Spacing();
            ImGui::Spacing();
            ImGui::Spacing();
            ImGui::Spacing();
            if (ImGui::Button("Step", ImVec2(120, 40))) {
                Step();
            }
            ImGui::Spacing();
            ImGui::Spacing();
            ImGui::Button("Run", ImVec2(120, 40));
            ImGui::Spacing();
            ImGui::Spacing();
            ImGui::Button("Reset", ImVec2(120, 40));
        }

        ImGui::End();

        log.Draw("Disassembly: History", &p_open);
    }

    void OnEvent(gfx::Event& e) override {}

    void OnUpdate() override {}
};

int main()
try {
    DebuggerGUI ui;
    ui.Run();
}
catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
}