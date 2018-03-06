#include <iostream>
#include <fstream>

#include "Assembler.h"

void Assembler::write_Rtype()
{
    std::cout << __PRETTY_FUNCTION__ << '\n';
    Token dest = advance();
    Token source = advance();
    Token target = advance();
    print_token(dest);
    print_token(source);
    print_token(target);
}

void Assembler::write_itype()
{
    std::cout << __PRETTY_FUNCTION__ << '\n';
}

const std::unordered_map<std::string, void (Assembler::*)()> Assembler::instructions {
        {"add",     &Assembler::write_Rtype},
        {"addi",    &Assembler::write_itype},
        {"addu",    &Assembler::write_Rtype},
        {"addiu",   &Assembler::write_itype},
        {"and",     &Assembler::write_Rtype},
        {"andi",    &Assembler::write_itype},
        {"div",     &Assembler::write_Rtype},
        {"divu",    &Assembler::write_Rtype},
        {"mult",    &Assembler::write_Rtype},
        {"multu",   &Assembler::write_Rtype},
        {"nor",     &Assembler::write_Rtype},
        {"or",      &Assembler::write_Rtype},
        {"ori",     &Assembler::write_itype},
        {"sll",     &Assembler::write_Rtype},
        {"sllv",    &Assembler::write_Rtype},
        {"sra",     &Assembler::write_Rtype},
        {"srav",    &Assembler::write_Rtype},
        {"srl",     &Assembler::write_Rtype},
        {"sub",     &Assembler::write_Rtype},
        {"subu",    &Assembler::write_Rtype},
        {"xor",     &Assembler::write_Rtype},
        {"xori",    &Assembler::write_itype},
        {"slt",     &Assembler::write_Rtype},
        {"sltu",    &Assembler::write_Rtype},
        {"slti",    &Assembler::write_itype},
        {"sltiu",   &Assembler::write_itype},
        {"beq",     &Assembler::write_itype},
        {"beq",     &Assembler::write_itype},
        {"bgtz",    &Assembler::write_itype},
        {"blez",    &Assembler::write_itype},
        {"bne",     &Assembler::write_itype},
        {"j",       &Assembler::write_Rtype},
        {"jal",     &Assembler::write_Rtype},
        {"jalr",    &Assembler::write_Rtype},
        {"jr",      &Assembler::write_Rtype},
        {"lb",      &Assembler::write_itype},
        {"lbu",     &Assembler::write_itype},
        {"lh",      &Assembler::write_itype},
        {"lhu",     &Assembler::write_itype},
        {"lw",      &Assembler::write_itype},
        {"lui",     &Assembler::write_itype},
        {"li",      &Assembler::write_itype},
        {"sb",      &Assembler::write_itype},
        {"sh",      &Assembler::write_itype},
        {"sw",      &Assembler::write_itype},
        {"mfhi",    &Assembler::write_itype},
        {"mflo",    &Assembler::write_itype},
        {"mtlo",    &Assembler::write_itype},
        {"mthi",    &Assembler::write_itype},
        {"syscall", &Assembler::write_itype},
};

inline Assembler::Assembler(Lexer& lexer)
        :lex(lexer), lookahead(lex.scan())
{ }

inline Assembler::~Assembler()
{
}

inline void Assembler::error(const std::string& msg)
{
    throw Parse_error("Error at " + lookahead.location().to_string() + ": " + msg);
}

inline void Assembler::error(Token token, const std::string& msg)
{
    throw Parse_error("Error at " + token.location().to_string() + ": " + msg);
}

inline Token Assembler::advance()
{
    auto prev = lookahead;
    lookahead = lex.scan();
    return prev;
}

inline bool Assembler::match(Tag tag)
{
    return lookahead.tag() == tag;
}

inline bool Assembler::match(char tag)
{
    return match(static_cast<Tag>(tag));
}

template<typename T, typename... Ts> bool Assembler::match(T tag, Ts... rest)
{
    return match(tag) || match(rest...);
}

inline Token Assembler::consume(char tag, const std::string& msg)
{
    return consume(static_cast<Tag>(tag), msg);
}

inline Token Assembler::consume(Tag tag, const std::string& msg)
{
    if (!match(tag))
        error(msg);

    return advance();
}

inline std::string Assembler::get_str(Token tok)
{
    return lex.table().at(tok.location()).str();
}

inline int Assembler::get_num(Token tok)
{
    return lex.table().at(tok.location()).num();
}

int Assembler::assemble()
{
    while (lookahead.tag() != Tag::Eof) {
        parse_line();
        advance();
    }
    return 0;
}

void Assembler::parse_line()
{
    switch (lookahead.tag()) {
    case Tag::Label:
        parse_label();
        break;
    case Tag::Directive:
        parse_directive();
        break;
    case Tag::Instruction:
        parse_instruction();
        break;
    case Tag::Eof:
    default:
        return;
    }
}

void Assembler::parse_label()
{
    std::cout << __PRETTY_FUNCTION__ << '\n';
    print_token(lookahead);
}

void Assembler::parse_instruction()
{
    std::cout << __PRETTY_FUNCTION__ << '\n';
    print_token(lookahead);
    (this->*instructions.at(get_str(lookahead)))();
}

void Assembler::parse_directive()
{
    std::cout << __PRETTY_FUNCTION__ << '\n';
    print_token(lookahead);
}

void Assembler::print_token(Token token)
{
    auto it = lex.table().find(lookahead.location());
    if (it != lex.table().end())
        std::cout << it->second << '\n';
}



std::ifstream open_file(const std::string& filename)
{
    std::ifstream res(filename);
    res.exceptions(std::ios::failbit);
    return res;
}

int main(int argc, char** argv)
{
    auto input = open_file("test.asm");
    Lexer lexer(input);
    Assembler assembler(lexer);
    return assembler.assemble();
}