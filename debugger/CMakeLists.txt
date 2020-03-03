project(debugger)
set (CMAKE_CXX_STANDARD 17)

add_executable(${PROJECT_NAME} debugger.cpp imgui_log.h cache.h)

target_link_libraries(${PROJECT_NAME} PUBLIC
        llvmes
)

set_target_properties(${PROJECT_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}/debugger
)

# Copy font file to the bin directory
add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_SOURCE_DIR}/fonts/verdana.ttf
        ${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}/debugger/verdana.ttf
)