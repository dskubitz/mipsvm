cmake_minimum_required(VERSION 3.9)
project(mips)

set(CMAKE_CXX_STANDARD 14)

add_definitions(-DDEBUG)
add_subdirectory(assembler)
add_subdirectory(linker)

add_executable(mips main.cpp mips.h debug.h common.h mips.cpp debug.cpp mem.h mem.cpp)