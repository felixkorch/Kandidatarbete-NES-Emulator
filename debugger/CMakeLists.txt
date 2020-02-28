project(debugger)

add_executable(${PROJECT_NAME} debugger.cpp imgui_log.h)

target_link_libraries(${PROJECT_NAME} PUBLIC
        llvmes
)

set_target_properties(${PROJECT_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}/debugger}
)