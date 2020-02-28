project(glad)

set(sources
    src/glad.c
)

add_library(${PROJECT_NAME} ${sources})
target_include_directories(${PROJECT_NAME}
        PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
            
)