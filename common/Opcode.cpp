#include "Opcode.h"
#include <ostream>

const std::unordered_map<std::string, Opcode,
                         case_insensitive_hash,
                         case_insensitive_equal> opcodes{ //NOLINT
        {"add",     Opcode::R_TYPE},
        {"addi",    Opcode::ADDI},
        {"addu",    Opcode::R_TYPE},
        {"addiu",   Opcode::ADDIU},
        {"and",     Opcode::R_TYPE},
        {"andi",    Opcode::ANDI},
        {"div",     Opcode::R_TYPE},
        {"divu",    Opcode::R_TYPE},
        {"mult",    Opcode::R_TYPE},
        {"multu",   Opcode::R_TYPE},
        {"nor",     Opcode::R_TYPE},
        {"or",      Opcode::R_TYPE},
        {"ori",     Opcode::ORI},
        {"sll",     Opcode::R_TYPE},
        {"sllv",    Opcode::R_TYPE},
        {"sra",     Opcode::R_TYPE},
        {"srav",    Opcode::R_TYPE},
        {"srlv",    Opcode::R_TYPE},
        {"srl",     Opcode::R_TYPE},
        {"sub",     Opcode::R_TYPE},
        {"subu",    Opcode::R_TYPE},
        {"xor",     Opcode::R_TYPE},
        {"xori",    Opcode::XORI},
        {"slt",     Opcode::R_TYPE},
        {"sltu",    Opcode::R_TYPE},
        {"slti",    Opcode::SLTI},
        {"sltiu",   Opcode::SLTIU},
        {"beq",     Opcode::BEQ},
        {"bne",     Opcode::BNE},
        {"bgtz",    Opcode::BGTZ},
        {"blez",    Opcode::BLEZ},
        {"j",       Opcode::J},
        {"jal",     Opcode::JAL},
        {"jalr",    Opcode::R_TYPE},
        {"jr",      Opcode::R_TYPE},
        {"lb",      Opcode::LB},
        {"lbu",     Opcode::LBU},
        {"lh",      Opcode::LH},
        {"lhu",     Opcode::LHU},
        {"lw",      Opcode::LW},
        {"lui",     Opcode::LUI},
        {"sb",      Opcode::SB},
        {"sh",      Opcode::SH},
        {"sw",      Opcode::SW},
        {"mfhi",    Opcode::R_TYPE},
        {"mflo",    Opcode::R_TYPE},
        {"mtlo",    Opcode::R_TYPE},
        {"mthi",    Opcode::R_TYPE},
        {"syscall", Opcode::R_TYPE},
        {"li",      Opcode::LI},
        {"move",    Opcode::MOVE},
        {"la",      Opcode::LA},
};

std::ostream& operator<<(std::ostream& os, Opcode code)
{
    switch (code) {
    case Opcode::J:
        return os << "j";
    case Opcode::JAL:
        return os << "jal";
    case Opcode::BEQ:
        return os << "beq";
    case Opcode::BNE:
        return os << "bne";
    case Opcode::BLEZ:
        return os << "blez";
    case Opcode::BGTZ:
        return os << "bgtz";
    case Opcode::ADDI:
        return os << "addi";
    case Opcode::ADDIU:
        return os << "addiu";
    case Opcode::SLTI:
        return os << "slti";
    case Opcode::SLTIU:
        return os << "sltiu";
    case Opcode::ANDI:
        return os << "andi";
    case Opcode::ORI:
        return os << "ori";
    case Opcode::XORI:
        return os << "xori";
    case Opcode::LUI:
        return os << "lui";
    case Opcode::LB:
        return os << "lb";
    case Opcode::LH:
        return os << "lh";
    case Opcode::LWL:
        return os << "lwl";
    case Opcode::LW:
        return os << "lw";
    case Opcode::LBU:
        return os << "lbu";
    case Opcode::LHU:
        return os << "lhu";
    case Opcode::LWR:
        return os << "lwr";
    case Opcode::SB:
        return os << "sb";
    case Opcode::SH:
        return os << "sh";
    case Opcode::SWL:
        return os << "swl";
    case Opcode::SW:
        return os << "sw";
    case Opcode::SWR:
        return os << "swr";
    case Opcode::LI:
        return os << "li";
    case Opcode::MOVE:
        return os << "move";
    case Opcode::LA:
        return os << "la";
    default:
        return os << "?";
    }
}
