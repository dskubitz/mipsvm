#ifndef MIPS_MIPSDEF_H
#define MIPS_MIPSDEF_H

#include <iosfwd>
#include <unordered_map>
#include "Hasher.h"

enum class Opcode : uint8_t {
    R_TYPE = 0,
    J = 2, JAL = 3,
    BEQ = 4, BNE = 5, BLEZ = 6, BGTZ = 7,
    ADDI = 8, ADDIU = 9, SLTI = 10, SLTIU = 11,
    ANDI = 12, ORI = 13, XORI = 14, LUI = 15,
    LB = 32, LH = 33, LWL = 34, LW = 35,
    LBU = 36, LHU = 37, LWR = 38,
    SB = 40, SH = 41, SWL = 42, SW = 43,
    SWR = 46, CACHE = 47,
    LL = 48, LWC1 = 49, LWC2 = 50, PREF = 51,
    LDC1 = 53, LDC2 = 54,
    SC = 56, SWC1 = 57, SWC2 = 58,
    SDC1 = 61, SDC2 = 62, LI = 65, MOVE = 66, LA = 67,
};

extern const std::unordered_map<std::string, Opcode,
                                case_insensitive_hash,
                                case_insensitive_equal> opcodes;
std::ostream& operator<<(std::ostream& os, Opcode code);

#endif //MIPS_MIPSDEF_H
