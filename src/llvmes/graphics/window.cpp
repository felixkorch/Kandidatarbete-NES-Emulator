#include "llvmes/graphics/window.h"

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include "glad/glad.h"

#include <iostream>

namespace llvmes::gfx {

Window::Window(int width, int height, const std::string& title, bool vsync)
    : m_width(width), m_height(height), m_vsync(vsync)
{
    if (!glfwInit())
        throw std::runtime_error("[Error] GLFW init failed");

    // This hint makes it work on mac
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // This hint makes scaling work on retina displays
#ifdef __APPLE__
    glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GLFW_TRUE);
#endif

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    SetVSync(false);

    GLFWwindow* window;
    window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        throw std::runtime_error("[ERROR] GLFW window creation failed");
    }

    glfwMakeContextCurrent(window);
    glfwSetWindowUserPointer(window, this);

    // Callback when the window gets resized
    glfwSetWindowSizeCallback(
        window, [](GLFWwindow* window, int width, int height) {
            Window* win = (Window*)glfwGetWindowUserPointer(window);
            win->event_handler(new WindowResizeEvent(width, height));
        });

    // Callback to set the viewport to match the new size of the window
    glfwSetFramebufferSizeCallback(
        window, [](GLFWwindow* window, int width, int height) {
            glViewport(0, 0, width, height);
        });

    // Mouse Events
    glfwSetScrollCallback(
        window, [](GLFWwindow* window, double x_offset, double y_offset) {
            Window* win = (Window*)glfwGetWindowUserPointer(window);
            win->event_handler(new MouseScrollEvent(x_offset, y_offset));
        });

    glfwSetCursorPosCallback(
        window, [](GLFWwindow* window, double x_pos, double y_pos) {
            Window* win = (Window*)glfwGetWindowUserPointer(window);
            win->event_handler(new MouseMoveEvent(x_pos, y_pos));
        });

    glfwSetMouseButtonCallback(
        window, [](GLFWwindow* window, int button, int action, int mods) {
            Window* win = (Window*)glfwGetWindowUserPointer(window);

            switch (action) {
                case GLFW_PRESS: {
                    win->event_handler(new MouseDownEvent(button));
                    break;
                }
                case GLFW_RELEASE: {
                    win->event_handler(new MouseReleaseEvent(button));
                    break;
                }
            }
        });

    // Key Events
    glfwSetCharCallback(window, [](GLFWwindow* window, unsigned int keycode) {
        Window* win = (Window*)glfwGetWindowUserPointer(window);
        win->event_handler(new KeyTypeEvent(keycode));
    });

    glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode,
                                  int action, int mods) {
        Window* win = (Window*)glfwGetWindowUserPointer(window);

        switch (action) {
            case GLFW_PRESS: {
                win->event_handler(new KeyPressEvent(key));
                break;
            }
            case GLFW_RELEASE: {
                win->event_handler(new KeyReleaseEvent(key));
                break;
            }
            case GLFW_REPEAT: {
                win->event_handler(new KeyRepeatEvent(key));
                break;
            }
        }
    });

    glfwSetWindowCloseCallback(window, [](GLFWwindow* window) {
        Window* win = (Window*)glfwGetWindowUserPointer(window);
        win->event_handler(new WindowCloseEvent);
    });

    // Initialize OpenGL for desktop
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        throw std::runtime_error("[ERROR] Failed to initialize glad");

    glEnable(GL_MULTISAMPLE);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    m_native_window = window;
}

Window::~Window()
{
    if (m_native_window != nullptr)
        glfwDestroyWindow((GLFWwindow*)m_native_window);
    glfwTerminate();
}

bool Window::IsClosed() const
{
    return glfwWindowShouldClose((GLFWwindow*)m_native_window);
}

void Window::SetVSync(bool enabled)
{
    enabled ? glfwSwapInterval(1) : glfwSwapInterval(0);
}

void Window::SetFullscreen()
{
    auto mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    glfwGetWindowPos((GLFWwindow*)m_native_window, &m_xpos, &m_ypos);
    glfwSetWindowMonitor((GLFWwindow*)m_native_window, glfwGetPrimaryMonitor(),
                         0, 0, mode->width, mode->height, GLFW_DONT_CARE);
    m_fullscreen = true;
}

void Window::SetWindowed()
{
    glfwSetWindowMonitor((GLFWwindow*)m_native_window, nullptr, m_xpos, m_ypos,
                         m_width, m_height, GLFW_DONT_CARE);
    m_fullscreen = false;
}

bool Window::UsingVSync()
{
    return m_vsync;
}

bool Window::IsFullScreen()
{
    return m_fullscreen;
}

void Window::Update()
{
    glfwSwapBuffers((GLFWwindow*)m_native_window);
    glfwPollEvents();
}

void Window::Clear(float r, float g, float b, float a)
{
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

}  // namespace llvmes::gfx