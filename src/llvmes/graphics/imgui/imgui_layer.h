#pragma once
#include "llvmes/graphics/window.h"

#include <imgui.h>

namespace llvmes {
namespace gfx {

// The functions in this class are supposed to be called after
// creating an OpenGL context with GLFW. The purpose for this class is
// to hide boilerplate code from this client side and to provide a clean
// interface for the user.
class ImGuiLayer {
   public:
    static void Create();
    static void Begin();
    static void End();
    static void Destroy();
    static void SetStyle(ImGuiStyle& style);
    static void ProcessEvents(Event& e);
};

}
}