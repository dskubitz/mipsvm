#include <iostream>
#include <fstream>
#include <sstream>

#include "Assembler.h"

inline Assembler::Assembler(Lexer& lexer)
        :lex(lexer), lookahead(lex.scan()) { }

inline void Assembler::error(Tag expected)
{
    std::stringstream ss;
    ss << "Error at " << lookahead.location()
       << ": expected " << expected << " but was " << lookahead.tag();
    throw Parse_error(ss.str());
}

inline Token Assembler::advance()
{
    auto prev = lookahead;
    lookahead = lex.scan();
    return prev;
}

inline bool Assembler::match(Tag tag)
{
    return lookahead.tag()==tag;
}

inline bool Assembler::match(char tag)
{
    return match(static_cast<Tag>(tag));
}

template<typename T, typename... Ts>
bool Assembler::match(T tag, Ts... rest)
{
    return match(tag) || match(rest...);
}

inline Token Assembler::consume(Tag tag)
{
    if (!match(tag))
        error(tag);

    return advance();
}

inline const std::string& Assembler::get_str(Token tok)
{
    return lex.table().at(tok.location()).str();
}

inline int Assembler::get_num(Token tok)
{
    return lex.table().at(tok.location()).num();
}

int Assembler::assemble()
{
    while (lookahead.tag()!=Tag::Eof) {
        parse_line();
        advance();
    }
    return 0;
}

void Assembler::parse_line()
{
    switch (lookahead.tag()) {
    case Tag::Label:parse_label();
        break;
    case Tag::Directive:parse_directive();
        break;
    case Tag::Instruction:parse_instruction();
        break;
    case Tag::Eof:
    default:return;
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

    const std::string& inst = get_str(lookahead);
    auto opcode = opcodes.at(inst);
    advance();

    if (opcode==Opcode::R_TYPE)
        parse_rtype(functs.at(inst));
    else if (as_integer(opcode)==2 || as_integer(opcode)==3)
        parse_jtype(opcode);
    else
        parse_itype(opcode);
}

void Assembler::parse_rtype(Funct funct)
{
    std::cout << __PRETTY_FUNCTION__ << '\n';

    switch (funct) {
    case Funct::SLL:
    case Funct::SRL:
    case Funct::SRA: {
        Token dest = consume(Tag::Register);
        Token source = consume(Tag::Register);
        Token imm = consume(Tag::Immediate);
        write_rtype(get_num(dest), get_num(source), 0, get_num(imm), funct);
        return;
    }
    case Funct::ADD:
    case Funct::ADDU:
    case Funct::SUB:
    case Funct::SUBU:
    case Funct::AND:
    case Funct::OR:
    case Funct::XOR:
    case Funct::NOR:
    case Funct::SLT:
    case Funct::SLTU:
    case Funct::SLLV:
    case Funct::SRLV:
    case Funct::SRAV: {
        Token dest = consume(Tag::Register);
        Token source = consume(Tag::Register);
        Token target = consume(Tag::Register);
        write_rtype(get_num(dest), get_num(source), get_num(target), 0, funct);
        return;
    }
    case Funct::MFLO:
    case Funct::MFHI: {
        Token dest = consume(Tag::Register);
        write_rtype(get_num(dest), 0, 0, 0, funct);
        return;
    }
    case Funct::MTLO:
    case Funct::MTHI: {
        Token source = consume(Tag::Register);
        write_rtype(0, get_num(source), 0, 0, funct);
        return;
    }
    case Funct::MULT:
    case Funct::MULTU:
    case Funct::DIV:
    case Funct::DIVU: {
        Token source = consume(Tag::Register);
        Token target = consume(Tag::Register);
        write_rtype(0, get_num(source), get_num(target), 0, funct);
        return;
    }

    case Funct::JR: {
        Token source = consume(Tag::Register);
        write_rtype(0, get_num(source), 0, 0, funct);
        return;
    }
    case Funct::JALR: {
        Token source = consume(Tag::Register);
        Token dest = consume(Tag::Register);
        write_rtype(get_num(dest), get_num(source), 0, 0, funct);
        return;
    }
    case Funct::MOVZ:
    case Funct::MOVN:
    case Funct::SYSCALL: {
        write_rtype(0, 0, 0, 0, funct);
        return;
    }
    case Funct::BREAK:
    case Funct::SYNC:
    case Funct::TGE:
    case Funct::TGEU:
    case Funct::TLT:
    case Funct::TLTU:
    case Funct::TEQ:
    case Funct::TNE:
        throw Parse_error("Unimplemented");
    }
}

void Assembler::parse_jtype(Opcode opcode)
{
    std::cout << __PRETTY_FUNCTION__ << '\n';

}

void Assembler::parse_itype(Opcode opcode)
{
    std::cout << __PRETTY_FUNCTION__ << '\n';

    switch (opcode) {
    case Opcode::LI:break;
    case Opcode::MOVE:break;

    case Opcode::BEQ:break;
    case Opcode::BNE:break;
    case Opcode::BLEZ:break;
    case Opcode::BGTZ:break;
    case Opcode::ADDI:break;
    case Opcode::ADDIU:break;
    case Opcode::SLTI:break;
    case Opcode::SLTIU:break;
    case Opcode::ANDI:break;
    case Opcode::ORI:break;
    case Opcode::XORI:break;
    case Opcode::LUI:break;
    case Opcode::LB:break;
    case Opcode::LH:break;
    case Opcode::LWL:break;
    case Opcode::LW:break;
    case Opcode::LBU:break;
    case Opcode::LHU:break;
    case Opcode::LWR:break;
    case Opcode::SB:break;
    case Opcode::SH:break;
    case Opcode::SWL:break;
    case Opcode::SW:break;
    case Opcode::SWR:break;

    case Opcode::CACHE:break;
    case Opcode::LL:break;
    case Opcode::LWC1:break;
    case Opcode::LWC2:break;
    case Opcode::PREF:break;
    case Opcode::LDC1:break;
    case Opcode::LDC2:break;
    case Opcode::SC:break;
    case Opcode::SWC1:break;
    case Opcode::SWC2:break;
    case Opcode::SDC1:break;
    case Opcode::SDC2:break;
        throw Parse_error("Unimplemented");

    }
}

void Assembler::write_rtype(unsigned int dest, unsigned int source, unsigned int target,
        unsigned int shamt, Funct funct)
{
    uint32_t inst = ((source & 31) << 21) | ((target & 31) << 16)
            | ((dest & 31) << 11) | ((shamt & 31) << 6)
            | (as_integer(funct) & 63);

    printf("0x%08x\n", inst);
}

void Assembler::parse_directive()
{
    std::cout << __PRETTY_FUNCTION__ << '\n';
    const std::string& directive = get_str(lookahead);
    std::cout << directive << '\n';
}

void Assembler::print_token(Token token)
{
    auto it = lex.table().find(lookahead.location());
    if (it!=lex.table().end())
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