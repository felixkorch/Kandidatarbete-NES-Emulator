project ("llvmes")
set (CMAKE_CXX_STANDARD 17)
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
add_subdirectory("debugger")