project(linker)

set(CMAKE_CXX_STANDARD 17)

add_executable(linker Linker.cpp Linker.h)
target_include_directories(linker PRIVATE ${common_SOURCE_DIR})
target_link_libraries(linker common)
