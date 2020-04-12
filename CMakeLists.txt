if (POLICY CMP0048)
cmake_policy(SET CMP0048 NEW)
endif (POLICY CMP0048)

project (llvmes)
set (CMAKE_CXX_STANDARD 11)
cmake_minimum_required (VERSION 3.8)


list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
find_package(LLVM REQUIRED CONFIG)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(VERBOSE)
    message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
    message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")
    message(STATUS "Include dirs in: ${LLVM_INCLUDE_DIRS}")

    foreach(lib ${LLVM_AVAILABLE_LIBS})
        message(STATUS ${lib})
    endforeach()
endif()

set(SOURCE
    src/llvmes/nes/rom.cpp
    src/llvmes/interpreter/cpu.cpp
    src/llvmes/interpreter/cpu.h
    src/llvmes/dynarec/disassembler.h
    src/llvmes/dynarec/6502_opcode.h
    src/llvmes/common.h
    src/llvmes/dynarec/compiler.h
    src/llvmes/dynarec/codegen.cpp
    src/llvmes/dynarec/compiler.cpp
    src/jitter/jitter.h
    src/jitter/jitter.cpp
)

add_library (${PROJECT_NAME} ${SOURCE})
target_include_directories(${PROJECT_NAME} PUBLIC
    ${LLVM_INCLUDE_DIRS}
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/src>
)
add_definitions(${LLVM_DEFINITIONS})

if(WIN32)
    target_link_libraries(${PROJECT_NAME} PUBLIC
        ${LLVM_AVAILABLE_LIBS}
    )
else()
    target_link_libraries(${PROJECT_NAME} PUBLIC
        LLVM
        dl
    )
endif()

if(VERBOSE)
    ######
    ######
endif()


add_subdirectory ("test")
add_subdirectory ("playground")

if(LLVMES_BUILD_DEBUGGER)
add_subdirectory("debugger")
endif()
