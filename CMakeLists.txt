cmake_minimum_required(VERSION 3.9)
project(mips C)

set(CMAKE_C_STANDARD 11)

add_definitions(-DDEBUG)
add_subdirectory(assembler)
add_subdirectory(linker)
add_executable(mips main.c mips.h debug.h common.h mips.c debug.c mem.h mem.c)