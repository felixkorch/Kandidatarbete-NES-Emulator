#pragma once
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
   public:
    Debugger();
    void OnEvent(gui::Event& e) override;
    void OnImGui() override;
    void Stop();
    void SaveBinary();
    void Step();
    void Reset();
    void Worker();
    void RunCPU();
    void OpenFile(const std::string& path);
    void ShowFileDialog();
    void ShowMenuBar();
    void ShowRegisterView();

   private:
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
};