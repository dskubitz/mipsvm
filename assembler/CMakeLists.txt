project(assembler)

set(CMAKE_CXX_STANDARD 14)
# see https://cmake.org/cmake/help/latest/module/FindBoost.html
find_package(Boost REQUIRED)

include_directories(${Boost_INCLUDE_DIR})
set(HEADERS Lexer.h Token.h Variant.h Assembler.h)
set(CORE_SRC ${HEADERS} Lexer.cpp Token.cpp Assembler.cpp)

add_executable(assembler ${CORE_SRC})
