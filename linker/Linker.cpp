#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>
#include <iomanip>
#include <Form.h>
#include <Bitmask.h>

#include "Linker.h"

template<typename T>
typename std::enable_if<std::is_integral<T>::value>::type
output_hex(std::ostream& os, T val)
{
    os << std::setfill('0') << std::hex << std::setw(2 * sizeof(T))
       << static_cast<int>(val) << std::dec << std::setfill(' ');
}

std::string chop_file_extension(const char* cstr)
{
    std::string str(cstr);
    str.erase(str.begin() + str.find('.'), str.end());
    return str;
}

int main(int argc, char** argv)
{
    if (argc < 2) {
        std::cerr << "usage: " << argv[0] << "file(s) ...\n";
        return 1;
    }
    return Linker{}.link(argc, argv);
}

int Linker::link(int argc, char** argv)
{
    const char* output_name = "axx.out";
    for (int i = 1; i < argc; ++i) {
        const char* arg = argv[i];
        if (strcmp(arg, "-o") == 0 && i + 1 < argc) {
            output_name = argv[i + 1];
            i += 1;
            continue;
        }
        std::ifstream input(arg);
        if (input.is_open()) {
            offsets.emplace_back(
                    chop_file_extension(arg),
                    text_segment.size() << 2,
                    data_segment.size() << 2);
            auto file = preprocess(input);
            read_file(file);
        } else {
            std::cerr << "failed to open file " << arg << ", aborting...\n";
            return 2;
        }
    }

    relocate_references();

    Form word(6, std::ios_base::hex, 8, '0');
    std::ofstream output(output_name, std::ios_base::trunc);
    output << ".text " << word(text_start << 2)
           << " .data " << word(data_start << 2)
           << " length " << word(static_cast<int>(linked.size() << 2)) << '\n';

    for (int i = 0; i < linked.size(); ++i) {
        output << word(linked[i]) << '\n';
    }
    return 0;
}

void Linker::relocate_references()
{
    text_start = 0;
    data_start = text_segment.size();

    // Concatenate text segment to data segment.
    linked.assign(text_segment.begin(), text_segment.end());
    for (int i = 0; i < data_segment.size(); i++)
        linked.push_back(data_segment[i].u);

    // Relocate all data label addresses
    for (auto& i : globals)
        if (i.second.segment == Segment::Data)
            i.second.address += data_start << 2;
    for (auto& frame : offsets)
        for (auto& i : frame.locals)
            if (i.second.segment == Segment::Data)
                i.second.address += data_start << 2;

    for (auto& item : relocation_info) {
        if (item.scope == "global") {
            try {
                resolve(globals.at(item.label), item);
            }
            catch (std::out_of_range& e) {
                std::cerr << "unresolved reference\n";
                exit(1);
            }
        } else {
            for (auto& frame : offsets) {
                if (item.scope == frame.name) {
                    resolve(frame.locals.at(item.label), item);
                }
            }
        }
    }
}

std::stringstream Linker::preprocess(std::istream& input)
{
    std::regex pattern(
            "[[:xdigit:]]{8}: [[:xdigit:]]{2} [[:xdigit:]]{2} [[:xdigit:]]{2} [[:xdigit:]]{2}"
                    "|[[:xdigit:]]{8}: [[:xdigit:]]{8}"
                    "|\\.text"
                    "|\\.data"
                    "|\\.symbol"
                    "|\\.relocation"
                    "|[[:xdigit:]]{8} \\w+ \\w+ \\w+ \\w+"
                    "|[[:xdigit:]]{8} \\w+ \\w+ \\w+"
    );
    std::stringstream ss;
    for (std::string line; std::getline(input, line);) {
        std::sregex_iterator it(line.begin(), line.end(), pattern), end = std::sregex_iterator();
        for (; it != end; ++it)
            ss << it->str() << '\n';
    }
    return ss;
}

void Linker::read_file(std::istream& input)
{
    enum {
        TEXT, DATA, SYMBOL, RELOCATION
    } section{};
    std::string name;
    input >> name;
    for (std::string line; std::getline(input, line);) {
        if (line == ".text")
            section = TEXT;
        else if (line == ".data")
            section = DATA;
        else if (line == ".symbol")
            section = SYMBOL;
        else if (line == ".relocation")
            section = RELOCATION;
        else {
            std::stringstream line_input(line);
            switch (section) {
            case TEXT:
                read_text_line(line_input);
                break;
            case DATA:
                read_data_line(line_input);
                break;
            case SYMBOL:
                read_symbol_line(line_input);
                break;
            case RELOCATION:
                read_relocation_line(line_input);
                break;
            }
        }
    }
}

void Linker::read_text_line(std::istream& input)
{
    std::string addr;
    std::string instr;
    if (input >> addr >> instr)
        text_segment.push_back(static_cast<uint32_t>(std::stoll(instr, nullptr, 16)));
}

void Linker::read_data_line(std::istream& input)
{
    std::string addr;
    std::string byte0, byte1, byte2, byte3;
    if (input >> addr >> byte0 >> byte1 >> byte2 >> byte3) {
        data_segment.emplace_back();
        auto& word = data_segment.back();
        word.b[0] = static_cast<uint8_t>(std::stoll(byte0, nullptr, 16));
        word.b[1] = static_cast<uint8_t>(std::stoll(byte1, nullptr, 16));
        word.b[2] = static_cast<uint8_t>(std::stoll(byte2, nullptr, 16));
        word.b[3] = static_cast<uint8_t>(std::stoll(byte3, nullptr, 16));
    }
}

void Linker::read_symbol_line(std::istream& input)
{
    std::string address, segment, visibility, label;
    if (input >> address >> segment >> visibility >> label) {
        Offset& offset = offsets.back();
        uint32_t addr = std::stoul(address, nullptr, 16);

        Segment seg = segment == "text" ? Segment::Text : Segment::Data;
        addr += seg == Segment::Text ? offset.text_offset : offset.data_offset;

        if (visibility == "local")
            offset.locals.insert({label, SymbolInfo(addr, seg)});
        else if (!globals.insert({label, SymbolInfo(addr, seg)}).second)
            std::cerr << "duplicate symbols\n";
    }
}

void Linker::read_relocation_line(std::istream& input)
{
    std::string address, segment, visibility, instruction, label;
    if (input >> address >> segment >> visibility >> instruction >> label) {
        Offset& offset = offsets.back();
        uint32_t addr = std::stoul(address, nullptr, 16);
        addr += segment == "text" ? offset.text_offset : offset.data_offset;

        auto it = offset.locals.find(label);
        if (it != offset.locals.end())
            relocation_info.emplace_back(addr, instruction, offset.name, label);
        else
            relocation_info.emplace_back(addr, instruction, "global", label);
    }
}

void Linker::resolve(SymbolInfo& symbol, RelocationInfo& reloc)
{
    Form hex8(6, std::ios_base::hex, 8, '0');
    switch (reloc.instruction) {
    case Opcode::J:
    case Opcode::JAL:
        linked.at(reloc.address >> 2) |= (symbol.address >> 2) & Bitmask<26>::value;
        break;
    case Opcode::ORI:
        linked.at(reloc.address >> 2) |= symbol.address & Bitmask<16>::value;
//        std::cout << reloc.instruction << ' ' << hex8(linked.at(reloc.address >> 2)) << '\n';
        break;
    case Opcode::LUI:
        linked.at(reloc.address >> 2) |= symbol.address & (Bitmask<16>::value << 16);
//        std::cout << reloc.instruction << ' ' << hex8(linked.at(reloc.address >> 2)) << '\n';
        break;
    default:
        break;
    }
}



