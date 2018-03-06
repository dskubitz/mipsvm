#ifndef MIPS_MIPS_H
#define MIPS_MIPS_H

#include "common.h"
#include "mem.h"

typedef uint32_t inst_t;

enum {
    OP_j = 2, OP_jal = 3,
    OP_beq = 4, OP_bne = 5, OP_blez = 6, OP_bgtz = 7,
    OP_addi = 8, OP_addiu = 9, OP_slti = 10, OP_sltiu = 11,
    OP_andi = 12, OP_ori = 13, OP_xori = 14, OP_lui = 15,
    OP_lb = 32, OP_lh = 33, OP_lwl = 34, OP_lw = 35,
    OP_lbu = 36, OP_lhu = 37, OP_lwr = 38,
    OP_sb = 40, OP_sh = 41, OP_swl = 42, OP_sw = 43,
    OP_swr = 46, OP_cache = 47,
    OP_ll = 48, OP_lwc1 = 49, OP_lwc2 = 50, OP_pref = 51,
    OP_ldc1 = 53, OP_ldc2 = 54,
    OP_sc = 56, OP_swc1 = 57, OP_swc2 = 58,
    OP_sdc1 = 61, OP_sdc2 = 62,
};

enum {
    F_sll = 0, F_srl = 2, F_sra = 3,
    F_sllv = 4, F_srlv = 6, F_srav = 7,
    F_jr = 8, F_jalr = 9, F_movz = 10, F_movn = 11,
    F_syscall = 12, F_break = 13, F_sync = 15,
    F_mfhi = 16, F_mthi = 17, F_mflo = 18, F_mtlo = 19,
    F_mult = 24, F_multu = 25, F_div = 26, F_divu = 27,
    F_add = 32, F_addu = 33, F_sub = 34, F_subu = 35,
    F_and = 36, F_or = 37, F_xor = 38, F_nor = 39,
    F_slt = 42, F_sltu = 43,
    F_tge = 48, F_tgeu = 49, F_tlt = 50, F_tltu = 51,
    F_teq = 52, F_tne = 54,
};

enum {
    F_add_f = 0, F_sub_f = 1, F_mul_f = 2, F_div_f = 3,
    F_sqrt_f = 4, F_abs_f = 5, F_mov_f = 6, F_neg_f = 7,
    F_round_w_f = 12, F_trunc_w_f = 13, F_ceil_w_f = 14, F_floor_w_f = 15,
    F_movz_f = 18, F_movn_f = 19,
    F_cvt_s_f = 32, F_cvt_d_f = 33, F_cvt_w_f = 36,
    F_c_f_f = 48, F_c_un_f = 49, F_c_eq_f = 50, F_c_ueq_f = 51,
    F_c_olt_f = 52, F_c_ult_f = 53, F_c_ole_f = 54, F_c_ule_f = 55,
    F_c_sf_f = 56, F_c_ngle_f = 57, F_c_seq_f = 58, F_c_ngl_f = 59,
    F_c_lt_f = 60, F_c_nge_f = 61, F_c_le_f = 62, F_c_ngt_f = 63,
};

enum {
    R_$zero = 0, R_$at = 1,
    R_$v0 = 2, R_$v1 = 3,
    R_$a0 = 4, R_$a1 = 5, R_$a2 = 6, R_$a3 = 7,
    R_$t0 = 8, R_$t1 = 9, R_$t2 = 10, R_$t3 = 11, R_$t4 = 12, R_$t5 = 13, R_$t6 = 14, R_$t7 = 15,
    R_$s0 = 16, R_$s1 = 17, R_$s2 = 18, R_$s3 = 19, R_$s4 = 20, R_$s5 = 21, R_$s6 = 22, R_$s7 = 23,
    R_$t8 = 24, R_$t9 = 25,
    R_$k0 = 26, R_$k1 = 27,
    R_$gp = 28, R_$sp = 29, R_$fp = 30, R_$ra = 31,
};



typedef struct core {
    union {
        struct {
            uint32_t zero, at,
                    v0, v1,
                    a0, a1, a2, a3,
                    t0, t1, t2, t3, t4, t5, t6, t7,
                    s0, s1, s2, s3, s4, s5, s6, s7,
                    t8, t9,
                    k0, k1,
                    gp, sp, fp, ra, pc, lo, hi;
        };
        uint32_t reg[35];
    };
} core_t;

void execute(memory_t* memory);

void dump_core(core_t* core);

#endif //MIPS_MIPS_H
