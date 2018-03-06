#ifndef MIPS_COMMON_H
#define MIPS_COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <limits.h>
#include <stdarg.h>
#include <string.h>

enum {
    MASK_5 = 31U,
    MASK_6 = 63U,
    MASK_8 = 255U,
    MASK_16 = 65535U,
    MASK_26 = 67108863U,
};
#define MASK_32 4294967295UL

inline static void error(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    fprintf(stderr, "error: ");
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n");
    va_end(args);
    exit(1);
}

inline static void* allocate(size_t sz)
{
    void* ptr = malloc(sz);
    if (!ptr)
        error("malloc");
    return ptr;
}

inline static void deallocate(void* ptr)
{
    free(ptr);
}

#define GET_OPCODE(inst, op) \
        (op = ((inst) & (63 << 26)) >> 26)

#define GET_I_FIELDS(inst, rs, rt, offset) \
    (rs = (((inst) & (31U << 21)) >> 21), \
    rt = (((inst) & (31U << 16)) >> 16), \
    offset = (((inst) & 65535U)))

#define GET_R_FIELDS(inst, rs, rt, rd, shamt, funct) \
    (rs = (((inst) & (31U << 21)) >> 21), \
    rt = (((inst) & (31U << 16)) >> 16), \
    rd = (((inst) & (31U << 11)) >> 11), \
    shamt = (((inst) & (31U << 6)) >> 6), \
    funct = (inst) & 63U)

#define GET_J_FIELDS(inst, imm) \
    (imm = inst & MASK_26)

#endif //MIPS_COMMON_H
