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
    assert(text_segment.size() == instructions.size());

    output << ".text\t\t\t\tsize: "
           << text_segment.size() << "\tsizeof: " << sizeof(uint32_t)
           << std::setw(8) << "\naddress" << std::setw(8) << "inst\n"
           << std::setw(8) << "-------- " << std::setw(9) << " --------\n";

    output << std::hex << std::setfill('0');
    int n = 0;
    for (auto& i : text_segment) {
        auto addr = static_cast<uint32_t>(n << 2);
        output << std::setw(8) << addr << ": " << std::setw(8)
               << i << "\t";
        auto it = addr_to_label.find(addr);
        if (it != addr_to_label.end())
            output << it->second.get() << ":\t" << instructions.at(n) << '\n';
        else
            output << "\t\t" << instructions.at(n) << '\n';
        n += 1;
    }
    output << '\n';

    output << std::dec << std::setfill(' ');
    output << ".data\t\t\t\tsize: " << data_segment.size() << "\tsizeof: " << sizeof(uint8_t)
           << std::setw(8) << "\naddress" << std::setw(8) << "data\n"
           << std::setw(8) << "-------- " << std::setw(9) << " -----------";

    output << std::hex << std::setfill('0');
    n = 0;
    for (auto& i : data_segment) {
        if (n % 4 == 0)
            output << "\n" << std::setw(8) << n << ": ";
        output << std::setw(2) << static_cast<int>(i) << ' ';
        n++;
    }
    output << std::dec << std::setfill(' ') << "\n\n";

    output << ".symbol\n";
    for (auto& i : symbol_table)
        output << i.second << ' ' << i.first << '\n';

    output << "\n.relocation\n";
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
    auto& identifier = get<std::string>(lookahead);
    auto address = current_address();
    if (address.seg == Segment::Text)
        addr_to_label.insert({address.addr, identifier});
    symbol_table.insert({identifier, address});
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
        auto dest = get<Reg>(consume(Tag::Register));
        auto source = get<Reg>(consume(Tag::Register));
        auto imm = get<int>(consume(Tag::Immediate));
        write_rtype(dest, Reg::ZERO, source, imm, funct);
        instructions.push_back(string_inst(funct, "  \t", dest, ',', source, ',', imm));
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
        auto dest = get<Reg>(consume(Tag::Register));
        auto source = get<Reg>(consume(Tag::Register));
        auto target = get<Reg>(consume(Tag::Register));
        write_rtype(dest, source, target, 0, funct);
        instructions.push_back(string_inst(funct, "  \t", dest, ',', source, ',', target));
        return;
    }
    case Funct::MFLO:
    case Funct::MFHI: {
        auto dest = get<Reg>(consume(Tag::Register));
        write_rtype(dest, Reg::ZERO, Reg::ZERO, 0, funct);
        instructions.push_back(string_inst(funct, "  \t", dest));
        return;
    }
    case Funct::MTLO:
    case Funct::MTHI: {
        auto source = get<Reg>(consume(Tag::Register));
        write_rtype(Reg::ZERO, source, Reg::ZERO, 0, funct);
        instructions.push_back(string_inst(funct, "  \t", source));
        return;
    }
    case Funct::MULT:
    case Funct::MULTU:
    case Funct::DIV:
    case Funct::DIVU: {
        auto source = get<Reg>(consume(Tag::Register));
        auto target = get<Reg>(consume(Tag::Register));
        write_rtype(Reg::ZERO, source, target, 0, funct);
        instructions.push_back(string_inst(funct, "  \t", source, ',', target));
        return;
    }
    case Funct::JR: {
        auto source = get<Reg>(consume(Tag::Register));
        write_rtype(Reg::ZERO, source, Reg::ZERO, 0, funct);
        instructions.push_back(string_inst(funct, "  \t", source));
        return;
    }
    case Funct::JALR: {
        auto source = get<Reg>(consume(Tag::Register));
        auto dest = get<Reg>(consume(Tag::Register));
        write_rtype(dest, source, Reg::ZERO, 0, funct);
        instructions.push_back(string_inst(funct, "  \t", dest, ',', source));
        return;
    }
    case Funct::SYSCALL: {
        // There is supposed to be exception information in this
        write_rtype(Reg::ZERO, Reg::ZERO, Reg::ZERO, 0, funct);
        instructions.push_back(string_inst(funct));
        return;
    }
    default:
        throw Parse_error("Unimplemented");
    }
}

void Assembler::parse_jtype(Opcode opcode)
{
    auto& identifier = get<std::string>(consume(Tag::Identifier));
    relocation_table[current_address()] = {opcode, identifier};
    write_jtype(opcode, 0);
    instructions.push_back(string_inst(opcode, "   \t", identifier));
}

void Assembler::parse_itype(Opcode opcode)
{
    // I'll try to keep pseudoinstructions near the front of this.
    switch (opcode) {
    case Opcode::LI: {
        auto dest = get<Reg>(consume(Tag::Register));
        auto imm = get<int>(consume(Tag::Immediate));

        if (imm > 65535) { // Wont fit in half-word
            write_itype(Opcode::LUI, Reg::AT, Reg::ZERO, (imm & 65535) << 16);
            write_itype(Opcode::ORI, dest, Reg::AT, imm & 65535);
            instructions.push_back(string_inst(opcode, "  \t", dest, ',', imm));
            instructions.emplace_back();
            return;
        }
        else {
            write_itype(Opcode::ADDIU, dest, Reg::ZERO, imm);
            instructions.push_back(string_inst(opcode, "  \t", dest, ',', imm));
        }
        return;
    }
    case Opcode::MOVE: {
        auto dest = get<Reg>(consume(Tag::Register));
        auto target = get<Reg>(consume(Tag::Register));
        write_rtype(dest, Reg::ZERO, target, 0, Funct::ADDU);
        instructions.push_back(string_inst(opcode, "  \t", dest, ',', target));

        return;
    }
    case Opcode::LA: {
        auto dest = get<Reg>(consume(Tag::Register));
        auto& ident = get<std::string>(consume(Tag::Identifier));

        relocation_table[current_address()] = {Opcode::LUI, ident};
        write_itype(Opcode::LUI, Reg::AT, Reg::ZERO, 0);

        relocation_table[current_address()] = {Opcode::ORI, ident};
        write_itype(Opcode::ORI, dest, Reg::AT, 0);

        instructions.push_back(string_inst(opcode, "  \t", dest, ',', ident));
        instructions.emplace_back();
        return;
    }
        // Branch instructions. These do not need to be entered in the
        // relocation table.
    case Opcode::BEQ:
    case Opcode::BNE:
    case Opcode::BLEZ:
    case Opcode::BGTZ: {
        auto dest = get<Reg>(consume(Tag::Register));
        auto source = get<Reg>(consume(Tag::Register));
        auto& identifier = get<std::string>(consume(Tag::Identifier));

        auto it = symbol_table.find(identifier);

        // We found a label a PC-relative branch takes
        if (it != symbol_table.end()) {
            auto address = it->second;

            if (address.seg == Segment::Data)
                throw Parse_error("branch to data segment");

            auto imm_val = (address.addr - text_address) >> 2;
            write_itype(opcode, source, dest, imm_val);
        }
        else { // We need to wait to resolve it
            branch_labels[identifier] = current_address();
            write_itype(opcode, source, dest, 0);
        }

        instructions.push_back(string_inst(opcode, "  \t", dest, ",", source, ",", identifier));
        return;
    }
    case Opcode::ADDIU:
    case Opcode::SLTIU:
    case Opcode::ADDI:
    case Opcode::SLTI:
    case Opcode::ANDI:
    case Opcode::ORI:
    case Opcode::XORI: {
        auto dest = get<Reg>(consume(Tag::Register));
        auto source = get<Reg>(consume(Tag::Register));
        int imm = 0;
        if (match(Tag::MINUS)) {
            advance();
            imm = -get<int>(consume(Tag::Immediate));
        }
        else {
            imm = get<int>(consume(Tag::Immediate));
        }
        write_itype(opcode, dest, source, imm);

        instructions.push_back(string_inst(opcode, "  \t", dest, ",", source, ",", imm));
        return;
    }
    case Opcode::LUI: {
        auto dest = get<Reg>(consume(Tag::Register));
        auto imm = get<int>(consume(Tag::Immediate));
        write_itype(opcode, dest, Reg::ZERO, imm);

        instructions.push_back(string_inst(opcode, "  \t", dest, ",", imm));
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
        auto dest = get<Reg>(consume(Tag::Register));
        int offset = 0;

        if (match(Tag::Immediate)) {
            offset = get<int>(advance());
        }
        auto source = get<Reg>(consume(Tag::Register));

        if (dest == Reg::GP
            || source == Reg::GP); // Need to figure out relocation info for lw/sw to vars in static area

        if (offset > 65535) {
            write_itype(Opcode::LUI, Reg::AT, Reg::ZERO, (offset & 65535) << 16);
            write_itype(Opcode::ORI, dest, Reg::AT, offset & 65535);
            if (offset)
                instructions.push_back(string_inst(opcode, "  \t", dest, ",", offset, '(', source, ')'));
            else
                instructions.push_back(string_inst(opcode, "  \t", dest, ",(", source, ')'));
            instructions.push_back("");

        }
        else {
            if (offset)
                instructions.push_back(string_inst(opcode, "  \t", dest, ",", offset, '(', source, ')'));
            else
                instructions.push_back(string_inst(opcode, "  \t", dest, ",(", source, ')'));
            write_itype(opcode, dest, source, offset);
        }

        return;
    }
    default:
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




