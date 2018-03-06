#ifndef MIPS_DEBUG_H
#define MIPS_DEBUG_H

#include "common.h"
#include <assert.h>

/*
#define PRINT_BITS(x) do {                              \
    __typeof__(x) X = (x);                                \
    for (int i = sizeof(X) * CHAR_BIT - 1; i >= 0; i--) { \
        fputc((X) & (1ULL << i) ? '1' : '0', stdout);     \
        if (i % 8 == 0)                                  \
            fputc(' ', stdout);                           \
    }                                                     \
    fputc('\n', stdout);                                  \
} while(0)
*/

#define PRINT_R_INST(x) do {                              \
    __typeof__(x) X = (x);                                \
    for (int i = sizeof(X) * CHAR_BIT - 1; i >= 0; i--) { \
        fputc((X) & (1ULL << i) ? '1' : '0', stdout);     \
        if (i == 26)                                      \
            fputc(' ', stdout);                           \
        else if (i == 21)                                 \
            fputc(' ', stdout);                           \
        else if (i == 16)                                 \
            fputc(' ', stdout);                           \
        else if (i == 11)                                 \
            fputc(' ', stdout);                           \
        else if (i == 6)                                  \
            fputc(' ', stdout);                           \
    }                                                     \
    fputc('\n', stdout);                                  \
    printf("opcode  rs    rt    rd   shamt  funct\n");    \
} while(0)

#define PRINT_I_INST(x) do {                              \
    __typeof__(x) X = (x);                                \
    for (int i = sizeof(X) * CHAR_BIT - 1; i >= 0; i--) { \
        fputc((X) & (1ULL << i) ? '1' : '0', stdout);     \
        if (i == 26)                                      \
            fputc(' ', stdout);                           \
        else if (i == 21)                                 \
            fputc(' ', stdout);                           \
        else if (i == 16)                                 \
            fputc(' ', stdout);                           \
    }                                                     \
    fputc('\n', stdout);                                  \
    printf("opcode  rs    rt      immediate   \n");       \
} while(0)

#define PRINT_J_INST(x) do {                              \
    __typeof__(x) X = (x);                                \
    for (int i = sizeof(X) * CHAR_BIT - 1; i >= 0; i--) { \
        fputc((X) & (1ULL << i) ? '1' : '0', stdout);     \
        if (i == 26)                                      \
            fputc(' ', stdout);                           \
    }                                                     \
    fputc('\n', stdout);                                  \
    printf("opcode          immediate         \n");       \
} while(0)

const char* reg_str(int r);
#endif //MIPS_DEBUG_H
