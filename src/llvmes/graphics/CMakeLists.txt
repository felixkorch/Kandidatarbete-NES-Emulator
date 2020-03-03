target_sources(llvmes PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/application.h
        ${CMAKE_CURRENT_LIST_DIR}/application.cpp
        ${CMAKE_CURRENT_LIST_DIR}/event.h
        ${CMAKE_CURRENT_LIST_DIR}/window.h
        ${CMAKE_CURRENT_LIST_DIR}/window.cpp
        ${CMAKE_CURRENT_LIST_DIR}/imgui/imgui_renderer.cpp
        ${CMAKE_CURRENT_LIST_DIR}/imgui/imgui_renderer.h
        ${CMAKE_CURRENT_LIST_DIR}/imgui/imgui_layer.h
        ${CMAKE_CURRENT_LIST_DIR}/imgui/imgui_layer.cpp
)