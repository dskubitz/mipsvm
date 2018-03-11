cmake_minimum_required(VERSION 3.9)
project(mips)

set(CMAKE_CXX_STANDARD 14)

add_subdirectory(assembler)
add_subdirectory(linker)
add_subdirectory(vm)
add_subdirectory(common)
include_directories(${NESTED_DIRS})
