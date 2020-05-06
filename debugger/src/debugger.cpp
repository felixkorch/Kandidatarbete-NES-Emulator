#include <llvmes-gui/application.h>
#include <llvmes-gui/log.h>
#include <llvmes/interpreter/cpu.h>
#include <llvmes/time.h>

#include <filesystem>
#include <fstream>
#include <iostream>

#include "basic_log.h"
#include "cache.h"
#include "imgui-filebrowser/imfilebrowser.h"
#include "imgui_memory_editor/imgui_memory_editor.h"

using namespace llvmes;

class Debugger : public gui::Application {
    volatile bool cpu_should_run = false;
    bool show_memory_editor = false;
    bool show_register_view = true;
    bool show_log_view = true;

    std::uint8_t x = 0, y = 0, a = 0, sp = 0;
    std::uint16_t pc = 0;

    llvmes::CPU cpu;
    std::vector<char> memory;

    BasicLog log;

    ImGui::FileBrowser file_dialog;
    std::vector<std::string> cache;
    MemoryEditor mem_edit;
    std::string loaded_file_path;

    ClockType start, stop;

   public:
    Debugger() : gui::Application(1000, 700, "LLVMES - Debugger"), memory(0x10000)
    {
        cpu.Read = [this](std::uint16_t addr) { return memory[addr]; };
        cpu.Write = [this](std::uint16_t addr, std::uint8_t data) {
            // Write to '0x2008' and 'A' will be written to stdout as char
            if (addr == 0x2008) {
                log.AddLog("%c", a);
            }
            // Write A to stdout
            else if (addr == 0x2009) {
                log.AddLog("%s\n", ToHexString(a).c_str());
            }
            // Write X to stdout
            else if (addr == 0x200A) {
                log.AddLog("%s\n", ToHexString(x).c_str());
            }
            // Write Y to stdout
            else if (addr == 0x200B) {
                log.AddLog("%s\n", ToHexString(y).c_str());
            }
            // Write status to stdout
            else if (addr == 0x200C) {
                log.AddLog("%s\n", ToHexString((uint8_t)cpu.reg_status).c_str());
            }
            // Exit program with exit code from reg A
            else if (addr == 0x200F) {
                stop = std::chrono::high_resolution_clock::now();
                cpu_should_run = false;
                LLVMES_TRACE("Program executed in: {}us",
                             GetDuration(TimeFormat::Micro, start, stop));
            }
            else {
                memory[addr] = data;
            }
        };
        cpu.Reset();

        // Load the recently opened files
        cache = RecentlyOpened::GetCache();
    }

    void OnEvent(gui::Event& e) override
    {
        if (e.GetEventType() == gui::EventType::KeyPressEvent) {
            gui::KeyPressEvent& ke = (gui::KeyPressEvent&)e;
            switch (ke.GetKeyCode()) {
                case LLVMES_KEY_M: {
                    show_memory_editor = !show_memory_editor;
                    break;
                }
                case LLVMES_KEY_R: {
                    show_register_view = !show_register_view;
                    break;
                }
                case LLVMES_KEY_L: {
                    show_log_view = !show_log_view;
                    break;
                }
                case LLVMES_KEY_O: {
                    file_dialog.SetTypeFilters({".bin"});
                    file_dialog.SetTitle("Open a binary file");
                    file_dialog.Open();
                    break;
                }
                default:
                    break;
            }
        }
        else if (e.GetEventType() == gui::EventType::DropEvent) {
            gui::DropEvent& de = (gui::DropEvent&)e;
            OpenFile(de.GetPath());
        }
    }

    void Stop() { cpu_should_run = false; }

    void SaveBinary()
    {
        fs::path p(loaded_file_path);
        std::string new_name = fs::path(p.parent_path() / p.stem()).string() + "_mod.bin";
        std::ofstream out(new_name, std::ios::binary | std::ios::out);
        out.write(memory.data(), memory.size());
        LLVMES_INFO("{} written!", new_name);
    }

    void Step()
    {
        cpu.Step();
        x = cpu.reg_x;
        y = cpu.reg_y;
        a = cpu.reg_a;
        sp = cpu.reg_sp;
        pc = cpu.reg_pc;
    }

    void Reset()
    {
        cpu.Reset();
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
            start = std::chrono::high_resolution_clock::now();
            while (cpu_should_run) {
                cpu.Step();
                x = cpu.reg_x;
                y = cpu.reg_y;
                a = cpu.reg_a;
                sp = cpu.reg_sp;
                pc = cpu.reg_pc;
            }
        });
        worker.detach();
    }

    void OpenFile(const std::string& path)
    {
        LLVMES_TRACE("Attempting to load file: {}", path);
        std::ifstream in{path, std::ios::binary};
        if (in.fail())
            throw std::runtime_error("Something went wrong with opening the file!");

        std::fill(memory.begin(), memory.end(),
                  0);  // Clear memory before loading new program.
        std::copy(std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>(),
                  memory.begin() + 0x8000);

        RecentlyOpened::Write(path);
        loaded_file_path = path;
        LLVMES_INFO("Successfully loaded '{}'", loaded_file_path);

        cpu.Reset();                         // Reset CPU and load reset-vector
        cache = RecentlyOpened::GetCache();  // Update cache
    }

    void ShowFileDialog()
    {
        file_dialog.Display();

        if (file_dialog.HasSelected()) {
            try {
                OpenFile(file_dialog.GetSelected().string());
            }
            catch (std::runtime_error& e) {
                LLVMES_ERROR(e.what());
            }

            file_dialog.ClearSelected();
        }
    }

    void ShowMenuBar()
    {
        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("Open", "O")) {
                    file_dialog.SetTypeFilters({".bin"});
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
                            if (ImGui::MenuItem(file_name.c_str())) {
                                try {
                                    OpenFile(line);
                                }
                                catch (std::runtime_error& e) {
                                    LLVMES_ERROR(e.what());
                                    cache = RecentlyOpened::GetCache();
                                }
                            }
                        }
                        ImGui::EndMenu();
                    }
                }
                if (ImGui::MenuItem("Save")) {
                    SaveBinary();
                }
                if (ImGui::MenuItem("Quit")) {
                    Terminate();
                }

                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("View")) {
                if (ImGui::MenuItem("Memory Editor", "M")) {
                    show_memory_editor = !show_memory_editor;
                }
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }
    }

    void ShowRegisterView()
    {
        ImGui::SetNextWindowPos(ImVec2(50, 80), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);
        ImGui::Begin("Register View", nullptr, ImGuiWindowFlags_NoTitleBar);
        ImGui::Text("Register View");

        ImGui::Columns(2, "outer", false);
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

        ImGui::Dummy(ImVec2(0.0f, 30.0f));
        if (ImGui::Button("Step", ImVec2(120, 0)))
            Step();
        ImGui::Dummy(ImVec2(0.0f, 20.0f));
        if (ImGui::Button(cpu_should_run ? "Stop" : "Run", ImVec2(120, 0))) {
            cpu_should_run ? Stop() : RunCPU();
        }
        ImGui::Dummy(ImVec2(0.0f, 20.0f));
        if (ImGui::Button("Reset", ImVec2(120, 0)))
            Reset();

        ImGui::End();
    }

    void OnImGui() override
    {
        ShowFileDialog();
        ShowMenuBar();
        if (show_register_view)
            ShowRegisterView();

        if (show_log_view)
            log.Draw("Log");

        if (show_memory_editor)
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