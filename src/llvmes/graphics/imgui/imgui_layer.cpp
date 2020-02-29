#include "llvmes/graphics/imgui/imgui_layer.h"
#include "llvmes/graphics/application.h"
#include "llvmes/graphics/event.h"
#include "llvmes/graphics/imgui/imgui_renderer.h"

#include <GLFW/glfw3.h>

namespace llvmes::gfx {

static float pixel_ratio_x, pixel_ratio_y;

void ImGuiLayer::Create()
{
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui::StyleColorsDark();

    io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
    io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;

    // io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    io.KeyMap[ImGuiKey_Tab] = LLVMES_KEY_TAB;
    io.KeyMap[ImGuiKey_LeftArrow] = LLVMES_KEY_LEFT;
    io.KeyMap[ImGuiKey_RightArrow] = LLVMES_KEY_RIGHT;
    io.KeyMap[ImGuiKey_UpArrow] = LLVMES_KEY_UP;
    io.KeyMap[ImGuiKey_DownArrow] = LLVMES_KEY_DOWN;
    io.KeyMap[ImGuiKey_PageUp] = LLVMES_KEY_PAGE_UP;
    io.KeyMap[ImGuiKey_PageDown] = LLVMES_KEY_PAGE_DOWN;
    io.KeyMap[ImGuiKey_Home] = LLVMES_KEY_HOME;
    io.KeyMap[ImGuiKey_End] = LLVMES_KEY_END;
    io.KeyMap[ImGuiKey_Insert] = LLVMES_KEY_INSERT;
    io.KeyMap[ImGuiKey_Delete] = LLVMES_KEY_DELETE;
    io.KeyMap[ImGuiKey_Backspace] = LLVMES_KEY_BACKSPACE;
    io.KeyMap[ImGuiKey_Space] = LLVMES_KEY_SPACE;
    io.KeyMap[ImGuiKey_Enter] = LLVMES_KEY_ENTER;
    io.KeyMap[ImGuiKey_Escape] = LLVMES_KEY_ESCAPE;
    io.KeyMap[ImGuiKey_A] = LLVMES_KEY_A;
    io.KeyMap[ImGuiKey_C] = LLVMES_KEY_C;
    io.KeyMap[ImGuiKey_V] = LLVMES_KEY_V;
    io.KeyMap[ImGuiKey_X] = LLVMES_KEY_X;
    io.KeyMap[ImGuiKey_Y] = LLVMES_KEY_Y;
    io.KeyMap[ImGuiKey_Z] = LLVMES_KEY_Z;

    ImGui_ImplOpenGL3_Init("#version 330");
    ImGuiStyle& style = ImGui::GetStyle();
    SetStyle(style);

    Application& app = Application::Get();

    int w, h;
    glfwGetFramebufferSize((GLFWwindow*)app.GetWindow().GetNativeWindow(), &w,
                           &h);

    int window_size_x = app.GetWindow().GetWidth();
    int window_size_y = app.GetWindow().GetHeight();

    pixel_ratio_x = (float)w / (float)window_size_x;
    pixel_ratio_y = (float)h / (float)window_size_y;

    io.Fonts->AddFontFromFileTTF("verdana.ttf", 18.0f * pixel_ratio_x, nullptr,
                                 nullptr);

    io.FontGlobalScale = 1.0f / pixel_ratio_x;
    ImGui::GetStyle().ScaleAllSizes(pixel_ratio_x);

    #ifndef NDEBUG
    LLVMES_TRACE("Framebuffer size ({}, {})", w, h);
    LLVMES_TRACE("Window size ({}, {})", window_size_x, window_size_y);
    LLVMES_TRACE("Pixel ratio: {}", pixel_ratio_x);
    #endif
}
void ImGuiLayer::SetStyle(ImGuiStyle& style)
{
    // https://github.com/ocornut/imgui/issues/707#issuecomment-415097227
    style.Colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.06f, 0.06f, 0.94f);
    style.Colors[ImGuiCol_ChildBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.00f);
    style.Colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
    style.Colors[ImGuiCol_Border] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
    style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.21f, 0.22f, 0.54f);
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.40f, 0.40f, 0.40f, 0.40f);
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.18f, 0.18f, 0.18f, 0.67f);
    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.29f, 0.29f, 0.29f, 1.00f);
    style.Colors[ImGuiCol_TitleBgCollapsed] =
        ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
    style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
    style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered] =
        ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrabActive] =
        ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
    style.Colors[ImGuiCol_CheckMark] = ImVec4(0.94f, 0.94f, 0.94f, 1.00f);
    style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
    style.Colors[ImGuiCol_SliderGrabActive] =
        ImVec4(0.86f, 0.86f, 0.86f, 1.00f);
    style.Colors[ImGuiCol_Button] = ImVec4(0.44f, 0.44f, 0.44f, 0.40f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.46f, 0.47f, 0.48f, 1.00f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.42f, 0.42f, 0.42f, 1.00f);
    style.Colors[ImGuiCol_Header] = ImVec4(0.70f, 0.70f, 0.70f, 0.31f);
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.70f, 0.70f, 0.70f, 0.80f);
    style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.48f, 0.50f, 0.52f, 1.00f);
    style.Colors[ImGuiCol_Separator] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
    style.Colors[ImGuiCol_SeparatorHovered] =
        ImVec4(0.72f, 0.72f, 0.72f, 0.78f);
    style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
    style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.91f, 0.91f, 0.91f, 0.25f);
    style.Colors[ImGuiCol_ResizeGripHovered] =
        ImVec4(0.81f, 0.81f, 0.81f, 0.67f);
    style.Colors[ImGuiCol_ResizeGripActive] =
        ImVec4(0.46f, 0.46f, 0.46f, 0.95f);
    style.Colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
    style.Colors[ImGuiCol_PlotLinesHovered] =
        ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.73f, 0.60f, 0.15f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogramHovered] =
        ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.87f, 0.87f, 0.87f, 0.35f);
    style.Colors[ImGuiCol_ModalWindowDarkening] =
        ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
    style.Colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
    style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
    style.Colors[ImGuiCol_NavWindowingHighlight] =
        ImVec4(1.00f, 1.00f, 1.00f, 0.70f);

    style.WindowPadding = ImVec2(8, 6);
    style.WindowRounding = 0.0f;
    style.FramePadding = ImVec2(5, 7);
    // style.FrameRounding            = 0.0f;
    style.ItemSpacing = ImVec2(5, 5);
    // style.ItemInnerSpacing         = ImVec2(1, 1);
    // style.TouchExtraPadding        = ImVec2(0, 0);
    // style.IndentSpacing            = 6.0f;
    // style.ScrollbarSize            = 12.0f;
    // style.ScrollbarRounding        = 16.0f;
    // style.GrabMinSize              = 20.0f;
    // style.GrabRounding             = 2.0f;
    // style.WindowTitleAlign.x = 0.50f;
    // style.FrameBorderSize = 0.0f;
    // style.WindowBorderSize = 1.0f;
}
void ImGuiLayer::Begin()
{
    ImGuiIO& io = ImGui::GetIO();
    Application& app = Application::Get();

    io.DisplaySize =
        ImVec2(app.GetWindow().GetWidth(), app.GetWindow().GetHeight());
    io.DisplayFramebufferScale = ImVec2(pixel_ratio_x, pixel_ratio_y);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();
}
void ImGuiLayer::End()
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
void ImGuiLayer::Destroy()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui::DestroyContext();
}
void ImGuiLayer::ProcessEvents(Event& e)
{
    ImGuiIO& io = ImGui::GetIO();
    if (e.GetEventType() == EventType::KeyPressEvent) {
        auto& ev = (KeyPressEvent&)e;

        io.KeysDown[ev.GetKeyCode()] = true;
        io.KeyCtrl = io.KeysDown[LLVMES_KEY_LEFT_CONTROL] ||
                     io.KeysDown[LLVMES_KEY_RIGHT_CONTROL];
        io.KeyShift = io.KeysDown[LLVMES_KEY_LEFT_SHIFT] ||
                      io.KeysDown[LLVMES_KEY_RIGHT_SHIFT];
        io.KeyAlt = io.KeysDown[LLVMES_KEY_LEFT_ALT] ||
                    io.KeysDown[LLVMES_KEY_RIGHT_ALT];
        io.KeySuper = io.KeysDown[LLVMES_KEY_LEFT_SUPER] ||
                      io.KeysDown[LLVMES_KEY_RIGHT_SUPER];
    }
    else if (e.GetEventType() == EventType::KeyReleaseEvent) {
        auto& ev = (KeyReleaseEvent&)e;

        io.KeysDown[ev.GetKeyCode()] = false;
    }
    else if (e.GetEventType() == EventType::KeyTypeEvent) {
        auto& ev = (KeyTypeEvent&)e;

        int keycode = ev.GetKeyCode();
        if (keycode > 0 && keycode < 0x10000)
            io.AddInputCharacter((unsigned short)keycode);
    }
    else if (e.GetEventType() == EventType::MouseMoveEvent) {
        auto& ev = (MouseMoveEvent&)e;

        auto xy = ev.GetPosition();
        io.MousePos = ImVec2(xy.first, xy.second);
    }
    else if (e.GetEventType() == EventType::MouseDownEvent) {
        auto& ev = (MouseDownEvent&)e;

        io.MouseDown[ev.GetButtonCode()] = true;
    }
    else if (e.GetEventType() == EventType::MouseReleaseEvent) {
        auto& ev = (MouseReleaseEvent&)e;

        io.MouseDown[ev.GetButtonCode()] = false;
    }
    else if (e.GetEventType() == EventType::MouseScrollEvent) {
        auto& ev = (MouseScrollEvent&)e;

        auto xy = ev.GetOffset();
        io.MouseWheelH += xy.first;
        io.MouseWheel += xy.second;
    }
    else if (e.GetEventType() == EventType::WindowResizeEvent) {
        auto& ev = (WindowResizeEvent&)e;

        auto size = ev.GetNewSize();
        io.DisplaySize = ImVec2(size.first, size.second);
        io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);
    }
}

}  // namespace llvmes::gfx