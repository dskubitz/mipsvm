#include "Assembler.h"
#include <fstream>

std::string chop_file_extension(const char* file)
{
    std::string res(file);
    size_t index = res.find('.');
    res.erase(res.begin() + index, res.end());
    return res;
}

int main(int argc, char** argv)
{
    if (argc != 2) {
        std::cerr << "usage: " << argv[0] << " file\n";
        return 1;
    }
    std::ifstream input(argv[1]);
    if (input.is_open()) {
        std::ofstream output(chop_file_extension(argv[1]) + ".o");
        Lexer lexer(input);
        Assembler assembler(lexer);
        return assembler.assemble(output);
    }
    std::cerr << "couldn't open file\n";
    return 1;
}

// For debugging, may remove in the future
void Assembler::print_data_segment()
{
    std::cout << " [ ";
    for (size_t i = 0; i < data_segment.size();) {
        auto c = data_segment[i++];
        if (isprint(c)) {
            std::cout << c << ' ';
        }
        else {
            std::cout << static_cast<int>(c) << ' ';
        }
        if (i % data_alignment == 0)
            std::cout << " | ";
    }
    std::cout << " ] \n";
}

int Assembler::assemble(std::ostream& output)
{
    while (lookahead.tag() != Tag::Eof) {
        try {
            parse_line();
        }
        catch (std::exception& e) {
            std::cerr << e.what() << '\n';
            synchronize();
        }
    }

    if (error_occured)
        return 1;

    backpatch_references();
    output_object(output);

    return 0;
}

void Assembler::backpatch_references()
{
    for (auto& i : branch_labels) {
        auto it = symbol_table.find(i.first);
        if (it != symbol_table.end())
            text_segment.at(i.second.addr >> 2) |= it->second.addr & 65535;
    }

    for (auto& i : globals) {
        auto it = symbol_table.find(i);
        if (it != symbol_table.end()) {
            symbol_table.at(it->first).vis = Visibility::Global;
        }
    }
}

void Assembler::output_object(std::ostream& output)
{
    output << ".text\t\t\t\tsize: "
           << text_segment.size() << "\tsizeof: " << sizeof(uint32_t)
           << std::setw(8) << "\naddress" << std::setw(8) << "inst\n"
           << std::setw(8) << "-------- " << std::setw(9) << " --------\n";
    output << std::hex << std::setfill('0');
    int n = 0;
    for (auto& i : text_segment) {
        output << std::setw(8) << n << ": " << std::setw(8) << i << '\n';
        n += 4;
    }
    output << std::dec << std::setfill(' ');

    output << ".data\t\t\t\tsize: " << data_segment.size() << "\tsizeof: " << sizeof(uint8_t)
           << std::setw(8) << "\naddress" << std::setw(8) << "data\n"
           << std::setw(8) << "-------- " << std::setw(9) << " --------";

    output << std::hex << std::setfill('0');
    n = 0;
    for (auto& i : data_segment) {
        if (n % 4 == 0)
            output << "\n" << std::setw(8) << n << ": ";
        output << std::setw(2) << static_cast<int>(i) << ' ';
        n++;
    }
    output << std::dec << std::setfill(' ') << '\n';

    output << ".symbol\n";
    for (auto& i : symbol_table)
        output << i.second << ' ' << i.first << '\n';

    output << ".relocation\n";
    for (auto& i : relocation_table) {
        output << i.first << std::internal << " " << i.second << '\n';
    }
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
        error(Tag::Instruction);
    }
}

void Assembler::parse_label()
{
    symbol_table.insert(std::make_pair(get<std::string>(lookahead), current_address()));
    advance();
}

void Assembler::parse_instruction()
{
    auto& inst = get<std::string>(lookahead);
    auto opcode = opcodes.at(inst);
    advance();

    if (opcode == Opcode::R_TYPE)
        parse_rtype(functs.at(inst));
    else if (as_integer(opcode) == 2 || as_integer(opcode) == 3)
        parse_jtype(opcode);
    else
        parse_itype(opcode);
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
        write_rtype(get<int>(dest), 0, get<int>(source), get<int>(imm), funct);
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
        write_rtype(get<int>(dest), get<int>(source), get<int>(target), 0, funct);
        return;
    }
    case Funct::MFLO:
    case Funct::MFHI: {
        Token dest = consume(Tag::Register);
        write_rtype(get<int>(dest), 0, 0, 0, funct);
        return;
    }
    case Funct::MTLO:
    case Funct::MTHI: {
        Token source = consume(Tag::Register);
        write_rtype(0, get<int>(source), 0, 0, funct);
        return;
    }
    case Funct::MULT:
    case Funct::MULTU:
    case Funct::DIV:
    case Funct::DIVU: {
        Token source = consume(Tag::Register);
        Token target = consume(Tag::Register);
        write_rtype(0, get<int>(source), get<int>(target), 0, funct);
        return;
    }
    case Funct::JR: {
        Token source = consume(Tag::Register);
        write_rtype(0, get<int>(source), 0, 0, funct);
        return;
    }
    case Funct::JALR: {
        Token source = consume(Tag::Register);
        Token dest = consume(Tag::Register);
        write_rtype(get<int>(dest), get<int>(source), 0, 0, funct);
        return;
    }
    case Funct::SYSCALL: {
        // There is supposed to be exception information in this
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
    Token identifier = consume(Tag::Identifier);
    relocation_table[current_address()] = {opcode, get<std::string>(identifier)};
    write_jtype(opcode, 0);
}

void Assembler::parse_itype(Opcode opcode)
{
    // I'll try to keep pseudoinstructions near the front of this.
    switch (opcode) {
    case Opcode::LI: {
        Token dest = consume(Tag::Register);
        Token imm = consume(Tag::Immediate);
        auto ival = get<int>(imm);
        if (ival > 65535) { // Wont fit in half-word
            write_itype(Opcode::LUI, as_unsigned(Reg::AT), 0, (ival & 65535) << 16);
            write_itype(Opcode::ORI, get<int>(dest), as_unsigned(Reg::AT), (ival & 65535));
            return;
        }
        else {
            write_itype(Opcode::ADDIU, get<int>(dest), 0, ival);
        }
        return;
    }
    case Opcode::MOVE: {
        Token dest = consume(Tag::Register);
        Token target = consume(Tag::Register);
        write_rtype(get<int>(dest), 0, get<int>(target), 0, Funct::ADDU);
        return;
    }
    case Opcode::LA: {
        Token dest = consume(Tag::Register);
        Token ident = consume(Tag::Identifier);
        write_itype(Opcode::LUI, as_unsigned(Reg::AT), 0, 0);
        relocation_table[current_address()] = {Opcode::LUI, get<std::string>(ident)};
        write_itype(Opcode::ORI, get<int>(dest), as_unsigned(Reg::AT), 0);
        relocation_table[current_address()] = {Opcode::ORI, get<std::string>(ident)};
        return;
    }
        // Branch instructions. These do not need to be entered in the
        // relocation table.
    case Opcode::BEQ:
    case Opcode::BNE:
    case Opcode::BLEZ:
    case Opcode::BGTZ: {
        Token dest = consume(Tag::Register);
        Token source = consume(Tag::Register);
        Token identifier = consume(Tag::Identifier);

        auto it = symbol_table.find(get<std::string>(identifier));

        // We found a label a PC-relative branch takes
        if (it != symbol_table.end()) {
            auto address = it->second;
            if (address.seg == Segment::Data)
                throw Parse_error("branch to data segment");
            auto imm_val = (address.addr - text_address) >> 2;
            write_itype(opcode, get<int>(source), get<int>(dest), imm_val);
        }
        else { // We need to wait to resolve it
            branch_labels[get<std::string>(identifier)] = current_address();
            write_itype(opcode, get<int>(source), get<int>(dest), 0);
        }
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
            imm = consume(Tag::Immediate);
            ival = -get<int>(imm);
        }
        else {
            imm = consume(Tag::Immediate);
            ival = get<int>(imm);
        }
        write_itype(opcode, get<int>(dest), get<int>(source), ival);
        return;
    }
    case Opcode::LUI: {
        Token dest = consume(Tag::Register);
        Token imm = consume(Tag::Immediate);
        write_itype(opcode, get<int>(dest), 0, get<int>(imm));
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
        int offset = 0;

        if (match(Tag::Immediate)) {
            Token imm = advance();
            offset = get<int>(imm);
        }
        Token source = consume(Tag::Register);
        if (offset > 65535) {
            write_itype(Opcode::LUI, as_unsigned(Reg::AT), 0, (offset & 65535) << 16);
            write_itype(Opcode::ORI, get<int>(dest), as_unsigned(Reg::AT), (offset & 65535));
        }

        write_itype(opcode, get<int>(dest), get<int>(source), offset);
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
        // These three are handled elsewhere, this is just to quiet compiler warning
    case Opcode::J:
    case Opcode::R_TYPE:
    case Opcode::JAL:
        throw Parse_error("Unimplemented");
    }
}

void Assembler::parse_directive()
{
    auto& directive = get<std::string>(lookahead);
    advance();

    Directive dir = directives.at(directive);
    switch (dir) {
    case Directive::ALIGN: {
        Token align = consume(Tag::Immediate);
        data_alignment = static_cast<uint8_t>(1 << get<int>(align));
        return;
    }
    case Directive::ASCII:
    case Directive::ASCIIZ: {
        if (current_segment != Segment::Data)
            throw Parse_error("declaring data in text segment");

        Token string = consume(Tag::String);
        auto& ascii = get<std::string>(string);

        auto align = data_alignment;

        // Size of string aligned to current alignment,
        // accounting whether or not it is null-terminated.
        size_t sz = dir == Directive::ASCIIZ ? ascii.size() + 1 : ascii.size();
        size_t end = sz / align * align;
        if (end < sz) end += align;

        // Add string to data segment
        std::copy(ascii.begin(), ascii.end(), std::back_inserter(data_segment));
        for (size_t i = 0; i <= end - sz; ++i) data_segment.emplace_back();

        // Next data label will be at this address
        data_address = static_cast<uint32_t>(data_segment.size());
        return;
    }
    case Directive::DATA:
        current_segment = Segment::Data;
        return;
    case Directive::TEXT:
        current_segment = Segment::Text;
        return;
    case Directive::GLOBL: {
        Token ident = consume(Tag::Identifier);
        auto& label = get<std::string>(ident);
        globals.push_back(label);
        return;
    }
    case Directive::BYTE: {
        if (current_segment != Segment::Data)
            throw Parse_error("declaring data in text segment");
        int written = 0;
        while (match(Tag::Immediate)) {
            Token imm = advance();
            auto val = static_cast<uint8_t>(get<int>(imm));
            data_segment.push_back(val);
            written += 1;
        }
        pad_remaining(written);
        return;
    }
    case Directive::HALF: {
        if (current_segment != Segment::Data)
            throw Parse_error("declaring data in text segment");
        int written = 0;
        while (match(Tag::Immediate)) {
            Token imm = advance();
            auto val = static_cast<uint16_t>(get<int>(imm));
            data_segment.push_back((val & (255 << 8)) >> 8);
            data_segment.push_back(val & 255);
            written += 2;
        }
        pad_remaining(written);
        return;
    }
    case Directive::WORD: {
        if (current_segment != Segment::Data)
            throw Parse_error("declaring data in text segment");
        int written = 0;
        while (match(Tag::Immediate)) {
            Token imm = advance();
            auto val = static_cast<uint16_t>(get<int>(imm));
            data_segment.push_back((val & (255 << 24)) >> 24);
            data_segment.push_back((val & (255 << 16)) >> 16);
            data_segment.push_back((val & (255 << 8)) >> 8);
            data_segment.push_back(val & 255);
            written += 4;
        }
        pad_remaining(written);
        return;
    }
    case Directive::EXTERN:
    case Directive::SPACE:
        break;
    }
}

void Assembler::pad_remaining(int written)
{
    int amt = written / data_alignment * data_alignment;
    if (amt < written) amt += data_alignment;
    for (int i = 0; i < amt - written; ++i)
        data_segment.emplace_back();

    data_address = static_cast<uint32_t>(data_segment.size());
}




