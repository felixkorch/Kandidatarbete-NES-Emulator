if (POLICY CMP0048)
    cmake_policy(SET CMP0048 NEW)
endif (POLICY CMP0048)

project (llvmes CXX)
set (CMAKE_CXX_STANDARD 17)
cmake_minimum_required (VERSION 3.8)


list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


option(GLFW_BUILD_TESTS OFF)
option(GLFW_BUILD_EXAMPLES OFF)
option(SPDLOG_BUILD_TESTS OFF)
option(SPDLOG_BUILD_EXAMPLES OFF)

add_library (${PROJECT_NAME})
add_subdirectory(ext) # Contains graphical dependencies
add_subdirectory(src/llvmes) # Source of llvmes

target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/src>
)

target_link_libraries(${PROJECT_NAME} PUBLIC
    imgui
    glfw
    spdlog
    glad
)

if(NOT WIN32)
    target_link_libraries(${PROJECT_NAME} PUBLIC dl)
endif()

add_subdirectory ("test")
add_subdirectory ("playground")
add_subdirectory("debugger")