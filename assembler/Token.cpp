//
// Created by dvsku on 2/26/2018.
//
#include "Token.h"
#include <ostream>

std::ostream& operator<<(std::ostream& os, const Source_location& location)
{
    return os << "Line " << location.line << " Column " << location.column;
}

std::ostream& operator<<(std::ostream& os, const Token& token)
{
    os << token.location();
    return os;
}

std::string Source_location::to_string() const
{
    return "Line " + std::to_string(line)
           + ", Column " + std::to_string(column);
}

const std::unordered_map<std::string, Opcode> opcodes { //NOLINT
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

const std::unordered_map<std::string, Tag> reserved_words { //NOLINT
        {"add",     Tag::Instruction},
        {"addi",    Tag::Instruction},
        {"addu",    Tag::Instruction},
        {"addiu",   Tag::Instruction},
        {"and",     Tag::Instruction},
        {"andi",    Tag::Instruction},
        {"div",     Tag::Instruction},
        {"divu",    Tag::Instruction},
        {"mult",    Tag::Instruction},
        {"multu",   Tag::Instruction},
        {"nor",     Tag::Instruction},
        {"or",      Tag::Instruction},
        {"ori",     Tag::Instruction},
        {"sll",     Tag::Instruction},
        {"sllv",    Tag::Instruction},
        {"sra",     Tag::Instruction},
        {"srav",    Tag::Instruction},
        {"srl",     Tag::Instruction},
        {"srlv",    Tag::Instruction},
        {"sub",     Tag::Instruction},
        {"subu",    Tag::Instruction},
        {"xor",     Tag::Instruction},
        {"xori",    Tag::Instruction},
        {"slt",     Tag::Instruction},
        {"sltu",    Tag::Instruction},
        {"slti",    Tag::Instruction},
        {"sltiu",   Tag::Instruction},
        {"beq",     Tag::Instruction},
        {"bgtz",    Tag::Instruction},
        {"blez",    Tag::Instruction},
        {"bne",     Tag::Instruction},
        {"j",       Tag::Instruction},
        {"jal",     Tag::Instruction},
        {"jalr",    Tag::Instruction},
        {"jr",      Tag::Instruction},
        {"lb",      Tag::Instruction},
        {"lbu",     Tag::Instruction},
        {"lh",      Tag::Instruction},
        {"lhu",     Tag::Instruction},
        {"lw",      Tag::Instruction},
        {"lui",     Tag::Instruction},
        {"li",      Tag::Instruction},
        {"la",      Tag::Instruction},
        {"sb",      Tag::Instruction},
        {"sh",      Tag::Instruction},
        {"sw",      Tag::Instruction},
        {"mfhi",    Tag::Instruction},
        {"mflo",    Tag::Instruction},
        {"mtlo",    Tag::Instruction},
        {"mthi",    Tag::Instruction},
        {"syscall", Tag::Instruction},
        {"move",    Tag::Instruction},
        {"li",      Tag::Instruction},
        {".align",  Tag::Directive},
        {".ascii",  Tag::Directive},
        {".asciiz", Tag::Directive},
        {".byte",   Tag::Directive},
        {".data",   Tag::Directive},
        {".extern", Tag::Directive},
        {".globl",  Tag::Directive},
        {".half",   Tag::Directive},
        {".space",  Tag::Directive},
        {".text",   Tag::Directive},
        {".word",   Tag::Directive},
        {"$zero",   Tag::Register},
        {"$0",      Tag::Register},
        {"$at",     Tag::Register},
        {"$1",      Tag::Register},
        {"$v0",     Tag::Register},
        {"$2",      Tag::Register},
        {"$v1",     Tag::Register},
        {"$3",      Tag::Register},
        {"$a0",     Tag::Register},
        {"$4",      Tag::Register},
        {"$a1",     Tag::Register},
        {"$5",      Tag::Register},
        {"$a2",     Tag::Register},
        {"$6",      Tag::Register},
        {"$a3",     Tag::Register},
        {"$7",      Tag::Register},
        {"$t0",     Tag::Register},
        {"$8",      Tag::Register},
        {"$t1",     Tag::Register},
        {"$9",      Tag::Register},
        {"$t2",     Tag::Register},
        {"$10",     Tag::Register},
        {"$t3",     Tag::Register},
        {"$11",     Tag::Register},
        {"$t4",     Tag::Register},
        {"$12",     Tag::Register},
        {"$t5",     Tag::Register},
        {"$13",     Tag::Register},
        {"$t6",     Tag::Register},
        {"$14",     Tag::Register},
        {"$t7",     Tag::Register},
        {"$15",     Tag::Register},
        {"$s0",     Tag::Register},
        {"$16",     Tag::Register},
        {"$s1",     Tag::Register},
        {"$17",     Tag::Register},
        {"$s2",     Tag::Register},
        {"$18",     Tag::Register},
        {"$s3",     Tag::Register},
        {"$19",     Tag::Register},
        {"$s4",     Tag::Register},
        {"$20",     Tag::Register},
        {"$s5",     Tag::Register},
        {"$21",     Tag::Register},
        {"$s6",     Tag::Register},
        {"$22",     Tag::Register},
        {"$s7",     Tag::Register},
        {"$23",     Tag::Register},
        {"$t8",     Tag::Register},
        {"$24",     Tag::Register},
        {"$t8",     Tag::Register},
        {"$25",     Tag::Register},
        {"$k0",     Tag::Register},
        {"$26",     Tag::Register},
        {"$k1",     Tag::Register},
        {"$27",     Tag::Register},
        {"$gp",     Tag::Register},
        {"$28",     Tag::Register},
        {"$sp",     Tag::Register},
        {"$29",     Tag::Register},
        {"$fp",     Tag::Register},
        {"$30",     Tag::Register},
        {"$ra",     Tag::Register},
        {"$31",     Tag::Register},
};

const std::unordered_map<std::string, Funct> functs { //NOLINT
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

std::ostream& operator<<(std::ostream& os, const Tag& tag)
{
    switch (tag) {
    case Tag::Identifier:
        return os << "identifier";
    case Tag::Label:
        return os << "label";
    case Tag::String:
        return os << "string";
    case Tag::Immediate:
        return os << "immediate";
    case Tag::Directive:
        return os << "directive";
    case Tag::Instruction:
        return os << "instruction";
    case Tag::Register:
        return os << "register";
    case Tag::Eof:
        return os << "EOF";
    case Tag::NL:
        return os << "NL";
    case Tag::MINUS:
        return os << '-';
    case Tag::COLON:
        return os << ':';
    }
}

const std::unordered_map<std::string, Reg> registers {
        {"$zero", Reg::ZERO},
        {"$0",    Reg::ZERO},
        {"$at",   Reg::AT},
        {"$1",    Reg::AT},
        {"$v0",   Reg::V0},
        {"$2",    Reg::V0},
        {"$v1",   Reg::V1},
        {"$3",    Reg::V1},
        {"$a0",   Reg::A0},
        {"$4",    Reg::A0},
        {"$a1",   Reg::A1},
        {"$5",    Reg::A1},
        {"$a2",   Reg::A2},
        {"$6",    Reg::A2},
        {"$a3",   Reg::A3},
        {"$7",    Reg::A3},
        {"$t0",   Reg::T0},
        {"$8",    Reg::T0},
        {"$t1",   Reg::T1},
        {"$9",    Reg::T1},
        {"$t2",   Reg::T2},
        {"$10",   Reg::T2},
        {"$t3",   Reg::T3},
        {"$11",   Reg::T3},
        {"$t4",   Reg::T4},
        {"$12",   Reg::T4},
        {"$t5",   Reg::T5},
        {"$13",   Reg::T5},
        {"$t6",   Reg::T6},
        {"$14",   Reg::T6},
        {"$t7",   Reg::T7},
        {"$15",   Reg::T7},
        {"$s0",   Reg::S0},
        {"$16",   Reg::S0},
        {"$s1",   Reg::S1},
        {"$17",   Reg::S1},
        {"$s2",   Reg::S2},
        {"$18",   Reg::S2},
        {"$s3",   Reg::S3},
        {"$19",   Reg::S3},
        {"$s4",   Reg::S4},
        {"$20",   Reg::S4},
        {"$s5",   Reg::S5},
        {"$21",   Reg::S5},
        {"$s6",   Reg::S6},
        {"$22",   Reg::S6},
        {"$s7",   Reg::S7},
        {"$23",   Reg::S7},
        {"$t8",   Reg::T8},
        {"$24",   Reg::T8},
        {"$t9",   Reg::T9},
        {"$25",   Reg::T9},
        {"$k0",   Reg::K0},
        {"$26",   Reg::K0},
        {"$k1",   Reg::K1},
        {"$27",   Reg::K1},
        {"$gp",   Reg::GP},
        {"$28",   Reg::GP},
        {"$sp",   Reg::SP},
        {"$29",   Reg::SP},
        {"$fp",   Reg::FP},
        {"$30",   Reg::FP},
        {"$ra",   Reg::RA},
        {"$31",   Reg::RA},
};

std::ostream& operator<<(std::ostream& os, Reg reg)
{
    switch (reg) {
    case Reg::ZERO:
        return os << "$zero";
    case Reg::AT:
        return os << "$at";
    case Reg::V0:
        return os << "$v0";
    case Reg::V1:
        return os << "$v1";
    case Reg::A0:
        return os << "$a0";
    case Reg::A1:
        return os << "$a1";
    case Reg::A2:
        return os << "$a2";
    case Reg::A3:
        return os << "$a3";
    case Reg::T0:
        return os << "$t0";
    case Reg::T1:
        return os << "$t1";
    case Reg::T2:
        return os << "$t2";
    case Reg::T3:
        return os << "$t3";
    case Reg::T4:
        return os << "$t4";
    case Reg::T5:
        return os << "$t5";
    case Reg::T6:
        return os << "$t6";
    case Reg::T7:
        return os << "$t7";
    case Reg::S0:
        return os << "$s0";
    case Reg::S1:
        return os << "$s1";
    case Reg::S2:
        return os << "$s2";
    case Reg::S3:
        return os << "$s3";
    case Reg::S4:
        return os << "$s4";
    case Reg::S5:
        return os << "$s5";
    case Reg::S6:
        return os << "$s6";
    case Reg::S7:
        return os << "$s7";
    case Reg::T8:
        return os << "$t8";
    case Reg::T9:
        return os << "$t9";
    case Reg::K0:
        return os << "$k0";
    case Reg::K1:
        return os << "$k1";
    case Reg::GP:
        return os << "$gp";
    case Reg::SP:
        return os << "$sp";
    case Reg::FP:
        return os << "$fp";
    case Reg::RA:
        return os << "$ra";
    }
}

const std::unordered_map<std::string, Directive> directives { //NOLINT
        {".align",  Directive::ALIGN},
        {".ascii",  Directive::ASCII},
        {".asciiz", Directive::ASCIIZ},
        {".byte",   Directive::BYTE},
        {".data",   Directive::DATA},
        {".extern", Directive::EXTERN},
        {".globl",  Directive::GLOBL},
        {".half",   Directive::HALF},
        {".space",  Directive::SPACE},
        {".text",   Directive::TEXT},
        {".word",   Directive::WORD},
};
