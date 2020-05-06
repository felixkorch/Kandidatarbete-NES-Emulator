set(PLAYGROUND_FILES
        #execute_program
        #6502_testsuite
        #disassemble_test
)

if(LLVMES_BUILD_GUI)
set(PLAYGROUND_FILES ${PLAYGROUND_FILES}
    gui_test
    bubblesort_vis
)
endif()

set(LLVM_PLAYGROUND_OUTPUT_DIR ${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}/playground)

foreach (file ${PLAYGROUND_FILES})

    add_executable(${file} ${file}.cpp)
    target_link_libraries(${file} PUBLIC
        llvmes
    )
    if(LLVMES_BUILD_GUI)
    target_link_libraries(${file} PUBLIC
        llvmes-gui
    )
    endif()

    set_target_properties(${file} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${LLVM_PLAYGROUND_OUTPUT_DIR}/${file}
    )    
endforeach()