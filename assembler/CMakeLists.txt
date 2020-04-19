add_executable(runasm runasm.cpp)

add_library(luajit STATIC IMPORTED)
set_target_properties(luajit PROPERTIES
  IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/LuaJIT/src/libluajit.a"
)

set (CMAKE_CXX_FLAGS "-pagezero_size 10000 -image_base 100000000")

target_link_libraries(runasm luajit)
target_include_directories(runasm PUBLIC LuaJIT/src)