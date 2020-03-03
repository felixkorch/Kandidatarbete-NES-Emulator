#include <llvmes/common/log.h>
#include <llvmes/graphics/application.h>

#include <imgui.h>
#include <iostream>


using namespace llvmes;

class Debugger : public gfx::Application {

    bool p_open = true;

   public:
    Debugger() : Application(1200, 800, "Test UI") {}

    void OnImGui() override
    {
        ImGui::ShowDemoWindow();
    }

    void OnEvent(gfx::Event& e) override
    {
        if (e.GetEventType() == gfx::EventType::KeyPressEvent) {
            auto& ev = (gfx::KeyPressEvent&)e;
            LLVMES_INFO("Key down: {}", ev.GetKeyCode());

            if (ev.GetKeyCode() == LLVMES_KEY_G) {
                LLVMES_TRACE("G key down!");
            }
        }
        else if(e.GetEventType() == gfx::EventType::MouseMoveEvent) {
            auto& ev = (gfx::MouseMoveEvent&)e;
            auto [x, y] = ev.GetPosition();
            LLVMES_TRACE("Mouse position: {} {}", x, y);
        }
    }

    void OnUpdate() override {}
};

int main() try
{
    Debugger ui;
    ui.Run();
}catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
}