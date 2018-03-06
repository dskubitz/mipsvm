#include "common.h"
#include "mips.h"
#include "debug.h"
#include "mem.h"

uint32_t R_inst(unsigned rs, unsigned rt, unsigned rd, unsigned shamt, unsigned func)
{
    rs &= MASK_5;
    rt &= MASK_5;
    rd &= MASK_5;
    shamt &= MASK_5;
    func &= MASK_6;
    return (rs << 21) | (rt << 16) | (rd << 11) | (shamt << 6) | func;
}

uint32_t I_inst(unsigned op, unsigned rs, unsigned rt, unsigned imm)
{
    op &= MASK_6;
    rs &= MASK_5;
    rt &= MASK_5;
    imm &= MASK_16;
    return (op << 26) | (rs << 21) | (rt << 16) | imm;
}

uint32_t J_inst(unsigned op, unsigned imm)
{
    assert(op == 2 || op == 3);
    op &= MASK_6;
    imm &= MASK_26;
    return (op << 26) | imm;
}

#define WRITE_INST(code, inst) (*(code)++ = (inst))

#define ADD(dest, source, target) R_inst((source), (target), (dest), 0, F_add)
#define ADD_UNSIGNED(dest, source, target) R_inst((source), (target), (dest), 0, F_addu)
#define SUB(dest, source, target) R_inst((source), (target), (dest), 0, F_sub)
#define SUB_UNSIGNED(dest, source, target) R_inst((source), (target), (dest), 0, F_subu)
#define SHIFT_LEFT_LOGICAL(dest, target, imm) R_inst(0, (target), (dest), (imm), F_sll)
#define JUMP_REGISTER(source) R_inst((source), 0, 0, 0, F_jr)

#define ADD_IMMEDIATE(target, source, imm) I_inst(OP_addi, (source), (target), (imm))
#define STORE_WORD(target, source, imm) I_inst(OP_sw, (source), (target), (imm))
#define LOAD_WORD(target, source, imm) I_inst(OP_lw, (source), (target), (imm))
#define LOAD_UPPER_IMM(target, imm) I_inst(OP_lui, 0, (target), (imm))
#define BRANCH_IF_EQ(target, source, imm) I_inst(OP_beq, (source), (target), (imm))
#define BRANCH_IF_NEQ(target, source, imm) I_inst(OP_bne, (source), (target), (imm))

#define JUMP(imm) J_inst(OP_j, (imm))
#define JUMP_AND_LINK(imm) J_inst(OP_jal, (imm))

#define NOP 0
#define HALT ~0

int main()
{
    size_t mem_size = 30, stack_seg = 30, data_seg = 20, text_seg = 0;

    memory_t* mem = create_memory(mem_size, stack_seg, data_seg, text_seg);
    inst_t* ptr = mem->text_segment;

    WRITE_INST(ptr, LOAD_UPPER_IMM(R_$s2, 0xCAFE));
    WRITE_INST(ptr, JUMP_AND_LINK(7));
    WRITE_INST(ptr, NOP);
    WRITE_INST(ptr, HALT);
    WRITE_INST(ptr, NOP);
    WRITE_INST(ptr, NOP);
    WRITE_INST(ptr, ADD_IMMEDIATE(R_$s0, R_$zero, 0));
    WRITE_INST(ptr, ADD_IMMEDIATE(R_$s1, R_$zero, 10));
    WRITE_INST(ptr, BRANCH_IF_EQ(R_$s0, R_$s1, 2));
    WRITE_INST(ptr, ADD_IMMEDIATE(R_$s0, R_$s0, 1));
    WRITE_INST(ptr, JUMP(8));
    WRITE_INST(ptr, JUMP_REGISTER(R_$ra));

    execute(mem);

    destroy_memory(mem);
    return 0;
}