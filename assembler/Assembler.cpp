#include <iostream>
#include <fstream>
#include <sstream>

#include "Assembler.h"

int Assembler::assemble()
{
    while (lookahead.tag() != Tag::Eof) {
        parse_line();
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
        if (current_segment != Segment::Text)
            throw Parse_error("code in data segment");
        parse_instruction();
        break;
    default:
        std::cout << "skipping " << lookahead.tag() << '\n';
        advance();
        return;
    }
}

void Assembler::parse_label()
{
    add_resolved(get_str(lookahead), current_address());
    for (auto& i : symbol_table) {
        std::cout << i.first << ": " << i.second.addr / 4 << '\n';
    }
    advance();
}

void Assembler::parse_instruction()
{
    const std::string& inst = get_str(lookahead);
    auto opcode = opcodes.at(inst);
    advance();

    if (opcode == Opcode::R_TYPE)
        parse_rtype(functs.at(inst));
    else if (as_integer(opcode) == 2 || as_integer(opcode) == 3)
        parse_jtype(opcode);
    else
        parse_itype(opcode);
    text_address += 1;
}

void Assembler::parse_rtype(Funct funct)
{
    switch (funct) {
    case Funct::SLL:
    case Funct::SRL:
    case Funct::SRA: {
        Token dest = consume(Tag::Register);
        Token source = consume(Tag::Register);
        Token imm = consume(Tag::Immediate);
        write_rtype(get_unsigned(dest), 0, get_unsigned(source), get_unsigned(imm), funct);
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
        write_rtype(get_unsigned(dest), get_unsigned(source), get_unsigned(target), 0, funct);
        return;
    }
    case Funct::MFLO:
    case Funct::MFHI: {
        Token dest = consume(Tag::Register);
        write_rtype(get_unsigned(dest), 0, 0, 0, funct);
        return;
    }
    case Funct::MTLO:
    case Funct::MTHI: {
        Token source = consume(Tag::Register);
        write_rtype(0, get_unsigned(source), 0, 0, funct);
        return;
    }
    case Funct::MULT:
    case Funct::MULTU:
    case Funct::DIV:
    case Funct::DIVU: {
        Token source = consume(Tag::Register);
        Token target = consume(Tag::Register);
        write_rtype(0, get_unsigned(source), get_unsigned(target), 0, funct);
        return;
    }
    case Funct::JR: {
        Token source = consume(Tag::Register);
        write_rtype(0, get_unsigned(source), 0, 0, funct);
        return;
    }
    case Funct::JALR: {
        Token source = consume(Tag::Register);
        Token dest = consume(Tag::Register);
        write_rtype(get_unsigned(dest), get_unsigned(source), 0, 0, funct);
        return;
    }
    case Funct::SYSCALL: {
        write_rtype(0, 0, 0, 0, funct);
        return;
    }
    case Funct::MOVZ:
    case Funct::MOVN:
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

}

void Assembler::parse_itype(Opcode opcode)
{
    switch (opcode) {
    case Opcode::LI: {
        Token dest = consume(Tag::Register);
        Token imm = consume(Tag::Immediate);
        unsigned ival = get_unsigned(imm);
        if (ival > 65535) {
            write_itype(Opcode::LUI, as_unsigned(Reg::AT), 0, (ival & 65535) << 16);
            write_itype(Opcode::ORI, get_unsigned(dest), as_unsigned(Reg::AT), (ival & 65535));
            return;
        }
        else {
            write_itype(Opcode::ADDIU, get_unsigned(dest), 0, ival);
        }
        return;
    }
    case Opcode::MOVE: {
        Token dest = consume(Tag::Register);
        Token target = consume(Tag::Register);
        write_rtype(get_unsigned(dest), 0, get_unsigned(target), 0, Funct::ADDU);
        return;
    }
    case Opcode::BEQ:
    case Opcode::BNE:
    case Opcode::BLEZ:
    case Opcode::BGTZ: {
        Token dest = consume(Tag::Register);
        Token source = consume(Tag::Register);
        Token imm = consume(Tag::Identifier);
        // TODO: Add to symbol table
        write_itype(opcode, get_unsigned(source), get_unsigned(dest), 0);
        return;
    }
    case Opcode::ADDIU:
    case Opcode::SLTIU:
    case Opcode::ADDI:
    case Opcode::SLTI:
    case Opcode::ANDI:
    case Opcode::ORI:
    case Opcode::XORI: {
        Token dest = consume(Tag::Register);
        Token source = consume(Tag::Register);
        Token imm;
        int ival;
        if (match(Tag::MINUS)) {
            advance();
            Token imm = consume(Tag::Immediate);
            ival = -get_unsigned(imm);
        }
        else {
            imm = consume(Tag::Immediate);
            ival = get_unsigned(imm);
        }
        write_itype(opcode, get_unsigned(dest), get_unsigned(source), ival);
        return;
    }
    case Opcode::LUI: {
        Token dest = consume(Tag::Register);
        Token imm = consume(Tag::Immediate);
        write_itype(opcode, get_unsigned(dest), 0, get_unsigned(imm));
        return;
    }
    case Opcode::LB:
    case Opcode::LH:
    case Opcode::LWL:
    case Opcode::LW:
    case Opcode::LBU:
    case Opcode::LHU:
    case Opcode::LWR:
    case Opcode::SB:
    case Opcode::SH:
    case Opcode::SWL:
    case Opcode::SW:
    case Opcode::SWR: {
        Token dest = consume(Tag::Register);
        if (match(Tag::Immediate, Tag::Identifier)) {
            Token offset = advance();
            // TODO: Add to symbol table
        }
        Token source = consume(Tag::Register);
        write_itype(opcode, get_unsigned(dest), get_unsigned(source), 0);
        return;
    }

    case Opcode::CACHE:
    case Opcode::LL:
    case Opcode::LWC1:
    case Opcode::LWC2:
    case Opcode::PREF:
    case Opcode::LDC1:
    case Opcode::LDC2:
    case Opcode::SC:
    case Opcode::SWC1:
    case Opcode::SWC2:
    case Opcode::SDC1:
    case Opcode::SDC2:
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

void Assembler::write_itype(Opcode opcode, unsigned dest, unsigned source, unsigned immediate)
{
    uint32_t inst = ((as_integer(opcode) & 63) << 26) | ((source & 31) << 21)
            | ((dest & 31) << 16) | (immediate & 65535);

    printf("0x%08x\n", inst);
}

void Assembler::parse_directive()
{
    const std::string& directive = get_str(lookahead);
    advance();
    switch (directives.at(directive)) {
//    case Directive::ALIGN:
    case Directive::ASCII:
    case Directive::ASCIIZ: {
        if (current_segment != Segment::Data)
            throw Parse_error("declaring data in text segment");

        Token string = consume(Tag::String);
        const std::string& asciiz = get_str(string);

        // Size of string aligned to current alignment
        auto align = data_alignment;
        size_t sz = asciiz.size()+1, end = sz / align * align;
        if (end < sz) end += align;

        std::copy(asciiz.begin(), asciiz.end(), std::back_inserter(data_segment));
        for (size_t i = 0; i <= end - sz; ++i) data_segment.emplace_back(0);

        std::cout << " [ ";
        for (size_t i = 0; i < data_segment.size(); i) {
            auto c= data_segment[i++];
            if (isprint(c)) {
                std::cout << c << ' ';
            }
            else {
                std::cout << static_cast<int>(c) << ' ';
            }
            if (i % 4 == 0)
                std::cout << " | ";
        }
        std::cout << " ] \n";

        data_address = static_cast<uint32_t>(data_segment.size());
        return;
    }
    case Directive::DATA:
        std::cout << "setting segment to data\n";
        current_segment = Segment::Data;
        return;
    case Directive::TEXT:
        std::cout << "setting segment to text\n";
        current_segment = Segment::Text;
        return;
    case Directive::GLOBL: {
        Token ident = consume(Tag::Identifier);
        const std::string& label = get_str(ident);
        std::cout << "setting " << label << " to globl\n";
        globals.insert(label);

        return;
    }
        /*
    case Directive::BYTE:
    case Directive::EXTERN:
    case Directive::HALF:
    case Directive::SPACE:
    case Directive::WORD:
        */
    default:
        break;
    }
}

int main(int argc, char** argv)
try
{
    std::ifstream input("test.asm");
    if (input.is_open()) {
        Lexer lexer(input);
        Assembler assembler(lexer);
        return assembler.assemble();
    }
    return 1;
}
catch (std::exception& e) {
    std::cerr << e.what() << '\n';
}