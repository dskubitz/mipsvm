//
// Created by dvsku on 2/20/2018.
//
#include "Lexer.h"
#include <sstream>

static int escaped_char(int ch)
{
    switch (ch) {
    case 'n':
        return '\n';
    case 't':
        return '\t';
    case '\\':
        return '\\';
    case '\"':
        return '\"';
    case '0':
        return '\0';
    default:
        return ch;
    }
}

const std::unordered_map<std::string, Tag> reserved_words {
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
        {"sub",     Tag::Instruction},
        {"subu",    Tag::Instruction},
        {"xor",     Tag::Instruction},
        {"xori",    Tag::Instruction},
        {"slt",     Tag::Instruction},
        {"sltu",    Tag::Instruction},
        {"slti",    Tag::Instruction},
        {"sltiu",   Tag::Instruction},
        {"beq",     Tag::Instruction},
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
        {"sb",      Tag::Instruction},
        {"sh",      Tag::Instruction},
        {"sw",      Tag::Instruction},
        {"mfhi",    Tag::Instruction},
        {"mflo",    Tag::Instruction},
        {"mtlo",    Tag::Instruction},
        {"mthi",    Tag::Instruction},
        {"syscall",    Tag::Instruction},
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

Lexer::Lexer(std::istream& input)
        :input_(input),
         line_(1),
         column_(0)
{ }

Token Lexer::scan()
{
    int ch = advance();
    while (ch != EOF && isspace(ch)) {
        ch = advance();
    }
    location_ = {line_, column_};
    switch (ch) {
    case '#':
        return comment(ch);
    case '"':
        return scan_string(ch);
    case '.':
        return scan_directive(ch);
    case '$':
        return scan_register(ch);
    default:
        if (ch == '_' || isalpha(ch))
            return scan_keyword_or_identifier(ch);
        else if (isdigit(ch))
            return scan_number(ch);
        else
            return make_token(ch);
    }
}

Token Lexer::scan_number(int ch)
{
    std::string lexeme(1, static_cast<char>(ch));

    while (isdigit(peek()))
        lexeme.push_back(static_cast<char>(advance()));
    table_[location_] = std::stoi(lexeme);
    return make_token(Tag::Immediate);
}

namespace {
bool isalnum_(int ch)
{
    return isalnum(ch) || ch == '_';
}
}

Token Lexer::scan_directive(int ch)
{
    std::string lexeme(1, static_cast<char>(ch));
    while (isalnum_(peek()))
        lexeme.push_back(static_cast<char>(ch = advance()));

    auto it = reserved_words.find(lexeme);
    if (it == reserved_words.end())
        error("malformed directive");
    table_[location_] = lexeme;
    return make_token(it->second);
}

Token Lexer::scan_keyword_or_identifier(int ch)
{
    std::string lexeme(1, static_cast<char>(ch));

    while (isalnum_(peek()))
        lexeme.push_back(static_cast<char>(ch = advance()));

    auto word = reserved_words.find(lexeme);

    if (word != reserved_words.end()) {
        table_[location_] = lexeme;
        return make_token(word->second);
    }
    else {
        if (peek() == ':') {
            advance();
            table_[location_] = lexeme;
            return make_token(Tag::Label);
        }
        table_[location_] = lexeme;
        return make_token(Tag::Identifier);
    }
}

Token Lexer::comment(int ch)
{
    do {
        ch = advance();
    }
    while (ch != '\n');
    return scan();
}

Token Lexer::scan_string(int ch)
{
    std::string lexeme;
    for (;;) {
        ch = advance();
        switch (ch) {
        case '\\':
            lexeme.push_back(static_cast<char>(escaped_char(advance())));
            continue;
        case '\"':
            break;
        default:
            lexeme.push_back(static_cast<char>(ch));
            continue;
        }
        break;
    }
    table_[location_] = lexeme;
    return make_token(Tag::String);
}

bool Lexer::match(int ch)
{
    if (peek() != ch)
        return false;
    advance();
    return true;
}

int Lexer::peek()
{
    return input_.peek();
}

int Lexer::advance()
{
    int ch = input_.get();
    ++column_;
    if (ch == '\n') {
        ++line_;
        column_ = 0;
    }
    return ch;
}

Token Lexer::scan_register(int ch)
{
    std::string lexeme(1, static_cast<char>(ch));
    while (isalnum_(peek()))
        lexeme.push_back(static_cast<char>(ch = advance()));
    auto it = reserved_words.find(lexeme);
    if (it == reserved_words.end())
        error("malformed register");
    table_[location_] = lexeme;
    return make_token(it->second);
}
