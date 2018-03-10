#include "mem.h"
#include "debug.h"

memory_t* create_memory(size_t sz, size_t stack_loc, size_t data_loc, size_t text_loc)
{
    assert(sz >= stack_loc && stack_loc > data_loc && data_loc > text_loc);
    memory_t* memory = (memory_t*) allocate(sizeof *memory);
    memory->data = (uint32_t*) allocate(sz * sizeof(uint32_t));
    memset(memory->data, 0, sz * sizeof(uint32_t));
    memory->stack_segment = memory->data + stack_loc;
    memory->data_segment = memory->data + data_loc;
    memory->text_segment = memory->data + text_loc;
    memory->size = sz;
    return memory;
}

void destroy_memory(memory_t* memory)
{
    deallocate(memory->data);
    deallocate(memory);
}

void dump_memory(memory_t* memory)
{
    uint32_t* code = memory->data;
    uint32_t* ptr = memory->data_segment;
    uint32_t* end = memory->stack_segment;
    printf("[ ");
    for (; ptr != end;) {
        uint32_t cell = *ptr++;
        printf("0x%08x:%d", ADDRESS_OF(ptr - code - 1), cell);
        if (ptr == end)
            break;
        printf(" | ");
    }
    printf(" ]\n");
}

void dump_instructions(memory_t* memory)
{
    uint32_t* code = memory->data;
    uint32_t* ptr = memory->text_segment;
    uint32_t* end = memory->data_segment;
    for (; ptr != end; ++ptr) {
        uint32_t cell = *ptr;
        uint32_t op;
        printf("Address: 0x%08x\n", ADDRESS_OF(ptr - code));
        switch (GET_OPCODE(cell, op)) {
        case 0:
            PRINT_R_INST(cell);
            break;

        case 2:
        case 3:
            PRINT_J_INST(cell);
            break;
        default:
            PRINT_I_INST(cell);
            break;
        }
        putchar(10);
    }
}
