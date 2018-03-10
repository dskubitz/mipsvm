#include <unistd.h>

#include "debug.h"
#include "mips.h"

#ifdef DEBUG
#define DEBUG_PRINT(...) printf(__VA_ARGS__)
#else
#define DEBUG_PRINT(...)
#endif

void execute(memory_t* memory)
{
    core_t core;
    memset(&core, 0, sizeof core);

    core.gp = ADDRESS_OF(memory->data_segment - memory->data);
    core.sp = core.fp = ADDRESS_OF(memory->stack_segment - memory->data - 1);

    inst_t* PC = memory->text_segment;
    inst_t* end = memory->data_segment;

    uint32_t op, rs, rt, rd, shamt, funct;
    int32_t imm;

    while (PC != end) {
        inst_t inst = *PC++;
        core.pc = ADDRESS_OF(PC - memory->data);
        DEBUG_PRINT("0x%08x: ", ADDRESS_OF(PC - memory->data - 1));

        if (inst == ~0) { //HALT
            dump_core(&core);
            dump_memory(memory);
            return;
        }

        if (!inst) //NOP
            continue;

        GET_OPCODE(inst, op);

        if (op == 2) {
            DEBUG_PRINT("j %d\n", imm);
            GET_J_FIELDS(inst, imm);
            PC = memory->data + imm;
        }
        else if (op == 3) {
            DEBUG_PRINT("jal %d\n", imm);
            GET_J_FIELDS(inst, imm);
            core.ra = ADDRESS_OF(PC - memory->data + 1);
            PC = memory->data + imm;
        }
        else if (op == 0) {
            GET_R_FIELDS(inst, rs, rd, rt, shamt, funct);
            switch (funct) {
            case F_addu:
            case F_add:
                DEBUG_PRINT("add(u) %s %s %s\n", reg_str(rt), reg_str(rs), reg_str(rd));
                core.reg[rt] = core.reg[rs] + core.reg[rd];
                break;
            case F_subu:
            case F_sub:
                DEBUG_PRINT("sub(u) %s %s %s\n", reg_str(rt), reg_str(rs), reg_str(rd));
                core.reg[rt] = core.reg[rs] - core.reg[rd];
                break;
            case F_divu:
            case F_div:
                DEBUG_PRINT("div(u) %s %s\n", reg_str(rt), reg_str(rs));
                core.lo = core.reg[rs] / core.reg[rt];
                core.hi = core.reg[rs] % core.reg[rt];
                break;
            case F_mult:
            case F_multu: {
                DEBUG_PRINT("mult(u) %s %s\n", reg_str(rt), reg_str(rs));
                uint64_t value = core.reg[rs] * core.reg[rt];
                core.hi = (uint32_t) value & (MASK_32 << 32U);
                core.lo = (uint32_t) value & MASK_32;
            }
                break;
            case F_sll:
                DEBUG_PRINT("sll %s %s %d\n", reg_str(rt), reg_str(rd), shamt);
                core.reg[rt] = core.reg[rd] << shamt;
                break;
            case F_sllv:
                DEBUG_PRINT("sllv %s %s %s\n", reg_str(rt), reg_str(rs), reg_str(rd));
                core.reg[rt] = core.reg[rs] << core.reg[rd];
                break;
            case F_jr:
                DEBUG_PRINT("jr %s\n", reg_str(rs));
                PC = memory->data + INDEX_OF(core.reg[rs]);
                break;
            case F_sra:
            case F_srl:
                DEBUG_PRINT("sr(l|a) %s %s %d\n", reg_str(rt), reg_str(rd), shamt);
                core.reg[rt] = core.reg[rd] >> shamt;
                break;
            case F_srav:
            case F_srlv:
                DEBUG_PRINT("sr(l|a)v %s %s %d\n", reg_str(rt), reg_str(rd), shamt);
                core.reg[rt] = core.reg[rs] >> core.reg[rd];
                break;
            case F_and:
                DEBUG_PRINT("and %s %s %d\n", reg_str(rt), reg_str(rd), shamt);
                core.reg[rt] = core.reg[rs] & core.reg[rd];
                break;
            case F_or:
                DEBUG_PRINT("or %s %s %d\n", reg_str(rt), reg_str(rd), shamt);
                core.reg[rt] = core.reg[rs] | core.reg[rd];
                break;
            case F_xor:
                DEBUG_PRINT("xor %s %s %d\n", reg_str(rt), reg_str(rd), shamt);
                core.reg[rt] = core.reg[rs] ^ core.reg[rd];
                break;
            case F_nor:
                DEBUG_PRINT("nor %s %s %d\n", reg_str(rt), reg_str(rd), shamt);
                core.reg[rt] = (uint32_t) !(core.reg[rs] | core.reg[rd]);
                break;
            case F_sltu:
            case F_slt:
                DEBUG_PRINT("slt %s %s %s\n", reg_str(rt), reg_str(rs), reg_str(rd));
                core.reg[rt] = (uint32_t) (core.reg[rs] < core.reg[rd]);
                break;
            case F_mfhi:
                DEBUG_PRINT("mfhi %s\n", reg_str(rd));
                core.reg[rd] = core.hi;
                break;
            case F_mthi:
                DEBUG_PRINT("mthi %s\n", reg_str(rs));
                core.hi = core.reg[rs];
                break;
            case F_mflo:
                DEBUG_PRINT("mflo %s\n", reg_str(rd));
                core.reg[rd] = core.hi;
                break;
            case F_mtlo:
                DEBUG_PRINT("mtlo %s\n", reg_str(rs));
                core.lo = core.reg[rs];
                break;
            case F_syscall:
                DEBUG_PRINT("syscall ", reg_str(rs));
                switch (core.v0) {
                case 1:
                    printf("%d", core.a0);
                    break;
                case 2: // print float
                case 3: // print double
                case 4: // print string
                    break;
                case 5:
                    scanf("%d", &core.v0); // NOLINT
                    break;
                case 6:  // read float
                case 7:  // read double
                case 8:  // read string
                case 9:  // sbrk
                case 10: // exit
                    error("unsupported syscall: %d", core.a0);
                case 11: // print char
                    putchar(core.a0);
                    break;
                case 12:
                    core.v0 = (uint32_t) getchar();
                    break;
                case 13: // open file descriptor
                case 14: // read file descriptor
                case 15: // write to file descriptor
                case 16: // close file descriptor
                case 17: // exit with value
                default:
                    error("unsupported syscall: %d", core.a0);
                }
                /*
            case F_break:
            case F_jalr:
            case F_movz:
            case F_movn:
            case F_sync:
                // Traps
            case F_tge:
            case F_tgeu:
            case F_tlt:
            case F_tltu:
            case F_teq:
            case F_tne:
                 */

            default:
                error("unsupported op: %d", op);
            }
        }
        else {
            GET_I_FIELDS(inst, rs, rt, imm);
            switch (op) {
            case OP_addiu:
            case OP_addi:
                DEBUG_PRINT("addi(u) %s %s %d\n", reg_str(rt), reg_str(rs), imm);
                core.reg[rt] = core.reg[rs] + imm;
                break;
            case OP_andi:
                DEBUG_PRINT("andi %s %s %d\n", reg_str(rt), reg_str(rs), imm);
                core.reg[rt] = core.reg[rs] & imm;
                break;
            case OP_xori:
                DEBUG_PRINT("xori %s %s %d\n", reg_str(rt), reg_str(rs), imm);
                core.reg[rt] = core.reg[rs] ^ imm;
                break;
            case OP_ori:
                DEBUG_PRINT("xori %s %s %d\n", reg_str(rt), reg_str(rs), imm);
                core.reg[rt] = core.reg[rs] | imm;
                break;
                // Branches
            case OP_beq:
                DEBUG_PRINT("beq %s %s %d\n", reg_str(rt), reg_str(rs), imm);
                if (core.reg[rs] == core.reg[rt]) PC += imm;
                break;
            case OP_bne:
                DEBUG_PRINT("bne %s %s %d\n", reg_str(rt), reg_str(rs), imm);
                if (core.reg[rs] != core.reg[rt]) PC += imm;
                break;
            case OP_blez:
                DEBUG_PRINT("blez %s %s %d\n", reg_str(rt), reg_str(rs), imm);
                if (core.reg[rs] <= core.reg[rt]) PC += imm;
                break;
            case OP_bgtz:
                DEBUG_PRINT("bgtz %s %s %d\n", reg_str(rt), reg_str(rs), imm);
                if (core.reg[rs] > core.reg[rt]) PC += imm;
                break;
            case OP_lui:
                DEBUG_PRINT("lui %s %d\n", reg_str(rt), imm);
                core.reg[rt] = (uint32_t) (imm << 16U);
                break;
            case OP_slti:
            case OP_sltiu:
                DEBUG_PRINT("slti(u) %s %s %d\n", reg_str(rt), reg_str(rs), imm);
                core.reg[rt] = (uint32_t) (core.reg[rs] < imm);
                break;
                // TODO: Proper alignment
            case OP_lwr:
            case OP_lw:
                DEBUG_PRINT("lw(r) %s %s %d\n", reg_str(rt), reg_str(rs), INDEX_OF(imm));
                core.reg[rt] = memory->data[INDEX_OF(core.reg[rs]) + INDEX_OF(imm)];
                break;
            case OP_lbu:
            case OP_lb:
                DEBUG_PRINT("lb(u) %s %s %d\n", reg_str(rt), reg_str(rs), INDEX_OF(imm));
                core.reg[rt] = MASK_8 & memory->data[INDEX_OF(core.reg[rs]) + INDEX_OF(imm)];
                break;
            case OP_lhu:
            case OP_lh:
                DEBUG_PRINT("lh(u) %s %s %d\n", reg_str(rt), reg_str(rs), INDEX_OF(imm));
                core.reg[rt] = MASK_16 & memory->data[INDEX_OF(core.reg[rs]) + INDEX_OF(imm)];
                break;
            case OP_lwl:
                core.reg[rt] =
                        (MASK_8 & memory->data[INDEX_OF(core.reg[rs]) + INDEX_OF(imm)]) << 24U;
                break;
            case OP_swr:
            case OP_sw:
                DEBUG_PRINT("sw(r) %s %s %d\n", reg_str(rt), reg_str(rs), INDEX_OF(imm));
                memory->data[INDEX_OF(core.reg[rt]) + INDEX_OF(imm)] = core.reg[rs];
                break;
            case OP_sb:
                DEBUG_PRINT("sb %s %s %d\n", reg_str(rt), reg_str(rs), INDEX_OF(imm));
                memory->data[INDEX_OF(core.reg[rt]) + INDEX_OF(imm)] = core.reg[rs] & MASK_8;
            case OP_sh:
                DEBUG_PRINT("sh %s %s %d\n", reg_str(rt), reg_str(rs), INDEX_OF(imm));
                memory->data[INDEX_OF(core.reg[rt]) + INDEX_OF(imm)] = core.reg[rs] & MASK_16;
                break;
            case OP_swl:
                DEBUG_PRINT("swl %s %s %d\n", reg_str(rt), reg_str(rs), INDEX_OF(imm));
                memory->data[INDEX_OF(core.reg[rt]) + INDEX_OF(imm)] =
                        (core.reg[rs] & MASK_8) << 24U;
                break;

                /*
            case OP_cache:
            case OP_ll:
            case OP_lwc1:
            case OP_lwc2:
            case OP_pref:
            case OP_ldc1:
            case OP_ldc2:
            case OP_sc:
            case OP_swc1:
            case OP_swc2:
            case OP_sdc1:
            case OP_sdc2:
                 */
            default:
                error("unsupported op: %d", op);
            }
        }
    }
}

void dump_core(core_t* core)
{
    putchar('\n');
    for (int i = 0; i < 32; i++) {
        printf("%5s=%4d (0x%08x)", reg_str(i), core->reg[i], core->reg[i]);
        if ((i + 1) % 4 == 0)
            putchar('\n');
    }
}


