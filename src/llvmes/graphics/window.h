#pragma once
#include "llvmes/graphics/event.h"

#include <string>

namespace llvmes::gfx {

class Window {
   public:
    explicit Window(int width = 800, int height = 600,
                    const std::string& title = "Default Title",
                    bool vsync = false);

    ~Window();

    bool IsClosed() const;
    void SetVSync(bool enabled);
    void SetFullscreen();
    void SetWindowed();
    bool UsingVSync();
    bool IsFullScreen();
    void Update();
    void Clear(float r = 0.1f, float g = 0.1f, float b = 0.1f, float a = 0.1f);

    void* GetNativeWindow() { return m_native_window; }
    int GetWidth() { return m_width; }
    int GetHeight() { return m_height; }

   public:
    typedef std::function<void(Event*)> EventHandlerPtr;
    EventHandlerPtr event_handler;

   private:
    int m_width, m_height;
    bool m_vsync;
    bool m_fullscreen = false;
    int m_xpos = 0, m_ypos = 0;
    void* m_native_window = nullptr;
};

}  // namespace llvmes::gfx