//
// Created by dvsku on 2/26/2018.
//
#include "Token.h"
#include <ostream>

std::ostream& operator<<(std::ostream& os, const SourceLocation& location)
{
    return os << "Line " << location.line << " Column " << location.column;
}

std::ostream& operator<<(std::ostream& os, const Token& token)
{
    os << token.location();
    return os;
}

std::string SourceLocation::to_string() const
{
    return "Line " + std::to_string(line)
           + ", Column " + std::to_string(column);
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
