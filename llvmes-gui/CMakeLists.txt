project(llvmes-gui CXX)
set (CMAKE_CXX_STANDARD 17)
cmake_minimum_required (VERSION 3.8)

add_subdirectory(glad)
add_subdirectory(glfw)
add_subdirectory(imgui)
add_subdirectory(spdlog)
add_subdirectory(glm)

add_library (${PROJECT_NAME}
    src/llvmes-gui/application.cpp
    src/llvmes-gui/application.h
    src/llvmes-gui/event.h
    src/llvmes-gui/window.cpp
    src/llvmes-gui/window.h
    src/llvmes-gui/log.h
    src/llvmes-gui/opengl.h
    src/llvmes-gui/log.cpp
    src/llvmes-gui/texture.cpp
    src/llvmes-gui/texture.h
    src/llvmes-gui/draw.h
    src/llvmes-gui/draw.cpp
    src/llvmes-gui/buffers.cpp
    src/llvmes-gui/buffers.h
    src/llvmes-gui/shader.cpp
    src/llvmes-gui/shader.h
    src/llvmes-gui/imgui/imgui_layer.cpp
    src/llvmes-gui/imgui/imgui_layer.h
    src/llvmes-gui/imgui/imgui_renderer.h
    src/llvmes-gui/imgui/imgui_renderer.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/src>
    ${glfw_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME} PUBLIC
    glad
    glfw
    imgui
    spdlog
    glm
)