project(assembler)

set(HEADERS Lexer.h Token.h Variant.h Assembler.h)
set(CORE_SRC ${HEADERS} Lexer.cpp Token.cpp Variant.cpp Assembler.cpp)

add_executable(assembler ${CORE_SRC})
