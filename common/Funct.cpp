#include "Funct.h"
#include <ostream>

const std::unordered_map<std::string, Funct> functs{ //NOLINT
        {"add",     Funct::ADD},
        {"addu",    Funct::ADDU},
        {"and",     Funct::AND},
        {"div",     Funct::DIV},
        {"divu",    Funct::DIVU},
        {"mult",    Funct::MULT},
        {"multu",   Funct::MULTU},
        {"nor",     Funct::NOR},
        {"or",      Funct::OR},
        {"sll",     Funct::SLL},
        {"sllv",    Funct::SLLV},
        {"sra",     Funct::SRA},
        {"srav",    Funct::SRAV},
        {"srl",     Funct::SRL},
        {"srlv",    Funct::SRLV},
        {"sub",     Funct::SUB},
        {"subu",    Funct::SUBU},
        {"xor",     Funct::XOR},
        {"slt",     Funct::SLT},
        {"sltu",    Funct::SLTU},
        {"jalr",    Funct::JALR},
        {"jr",      Funct::JR},
        {"mfhi",    Funct::MFHI},
        {"mflo",    Funct::MFLO},
        {"mtlo",    Funct::MTHI},
        {"mthi",    Funct::MTHI},
        {"syscall", Funct::SYSCALL},
};

std::ostream& operator<<(std::ostream& os, Funct funct)
{
    switch (funct) {
    case Funct::SLL:
        return os << "sll";
    case Funct::SRL:
        return os << "srl";
    case Funct::SRA:
        return os << "sra";
    case Funct::SLLV:
        return os << "sllv";
    case Funct::SRLV:
        return os << "srlv";
    case Funct::SRAV:
        return os << "srav";
    case Funct::JR:
        return os << "jr";
    case Funct::JALR:
        return os << "jalr";
    case Funct::SYSCALL:
        return os << "syscall";
    case Funct::MFHI:
        return os << "mfhi";
    case Funct::MTHI:
        return os << "mthi";
    case Funct::MFLO:
        return os << "mflo";
    case Funct::MTLO:
        return os << "mtlo";
    case Funct::MULT:
        return os << "mult";
    case Funct::MULTU:
        return os << "multu";
    case Funct::DIV:
        return os << "div";
    case Funct::DIVU:
        return os << "divu";
    case Funct::ADD:
        return os << "add";
    case Funct::ADDU:
        return os << "addu";
    case Funct::SUB:
        return os << "sub";
    case Funct::SUBU:
        return os << "subu";
    case Funct::AND:
        return os << "and";
    case Funct::OR:
        return os << "or";
    case Funct::XOR:
        return os << "xor";
    case Funct::NOR:
        return os << "nor";
    case Funct::SLT:
        return os << "slt";
    case Funct::SLTU:
        return os << "sltu";
    default:
        return os << "?";
    }
}
