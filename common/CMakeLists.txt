project(common)
set(SRC Opcode.cpp Opcode.h Funct.cpp Funct.h Register.cpp Register.h Bitmask.h Form.h)
add_library(${PROJECT_NAME} STATIC ${SRC})
