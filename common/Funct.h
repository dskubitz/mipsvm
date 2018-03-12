#ifndef MIPS_FUNCT_H
#define MIPS_FUNCT_H

#include <unordered_map>
#include <iosfwd>
#include "Hasher.h"

enum class Funct : uint8_t {
    SLL = 0, SRL = 2, SRA = 3,
    SLLV = 4, SRLV = 6, SRAV = 7,
    JR = 8, JALR = 9, MOVZ = 10, MOVN = 11,
    SYSCALL = 12, BREAK = 13, SYNC = 15,
    MFHI = 16, MTHI = 17, MFLO = 18, MTLO = 19,
    MULT = 24, MULTU = 25, DIV = 26, DIVU = 27,
    ADD = 32, ADDU = 33, SUB = 34, SUBU = 35,
    AND = 36, OR = 37, XOR = 38, NOR = 39,
    SLT = 42, SLTU = 43,
    TGE = 48, TGEU = 49, TLT = 50, TLTU = 51,
    TEQ = 52, TNE = 54,
};

extern const std::unordered_map<std::string, Funct,
                                case_insensitive_hash,
                                case_insensitive_equal> functs;

std::ostream& operator<<(std::ostream& os, Funct funct);

#endif //MIPS_FUNCT_H
