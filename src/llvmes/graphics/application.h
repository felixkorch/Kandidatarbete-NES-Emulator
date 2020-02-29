#pragma once
#include <queue>
#include <string>

#include "llvmes/graphics/event.h"
#include "llvmes/graphics/log.h"
#include "llvmes/graphics/window.h"

namespace llvmes::gfx {

class Application {
   public:
    explicit Application(int width = 800, int height = 600,
                         const std::string& program_name = "Default Title");
    ~Application();

    virtual void OnImGui();
    virtual void OnUpdate();
    virtual void OnEvent(Event& e);

    void Run();
    void Terminate();

    static Application& Get();
    Window& GetWindow() { return *m_window; }

   private:
    bool m_running;
    std::unique_ptr<Window> m_window;
    std::queue<Event*> m_event_queue;

   private:
    static Application* s_application;
};

}  // namespace llvmes::gfx