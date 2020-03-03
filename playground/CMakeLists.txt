set(PLAYGROUND_FILES
    read_rom
    execute_program
    execute_file
    6502_testsuite
)

if(LLVMES_BUILD_GRAPHICS)
    set(PLAYGROUND_FILES ${PLAYGROUND_FILES}
        imgui_demo
    )
endif()

set(LLVM_PLAYGROUND_OUTPUT_DIR ${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}/playground)

foreach (file ${PLAYGROUND_FILES})

    add_executable(${file} ${file}.cpp)
    target_link_libraries(${file} PUBLIC
        llvmes
    )

    set_target_properties(${file} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${LLVM_PLAYGROUND_OUTPUT_DIR}
    )    
endforeach()