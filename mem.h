#ifndef MIPS_MEM_H
#define MIPS_MEM_H

#include "common.h"

typedef struct {
    size_t size;
    uint32_t *data;
    uint32_t *stack_segment;
    uint32_t *data_segment;
    uint32_t *text_segment;
} memory_t ;

memory_t* create_memory(size_t sz, size_t stack_loc, size_t data_loc, size_t text_loc);
void destroy_memory(memory_t* memory);
void dump_memory(memory_t* memory);
void dump_instructions(memory_t* memory);

#define ADDRESS_OF(addr) (uint32_t)((addr) << 2)

#define INDEX_OF(addr) (uint32_t)((addr) >> 2)

#endif //MIPS_MEM_H
