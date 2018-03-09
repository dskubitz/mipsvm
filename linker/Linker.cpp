#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>
#include <iomanip>

#include "Linker.h"

template<typename T>
typename std::enable_if<std::is_integral<T>::value>::type
output_hex(T val)
{
    std::cout << std::setfill('0') << std::hex << std::setw(2 * sizeof(T))
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
    for (int i = 1; i < argc; ++i) {
        const char* arg = argv[i];
        std::ifstream input(arg);
        if (input.is_open()) {
            auto obj_name = chop_file_extension(arg);
            text_offset = text_segment.size() << 2;
            data_offset = data_segment.size();
            auto file = preprocess(input);
            read_file(file);
        }
        else {
            std::cerr << "failed to open file " << arg << ", aborting...\n";
            return 2;
        }
    }
    for (int i = 0; i < text_segment.size();) {
        output_hex(i << 2);
        std::cout << ": ";
        output_hex(text_segment[i++]);
        std::cout << '\n';
    }
    for (int i = 0; i < data_segment.size();) {
        if (i % 4 == 0) {
            std::cout << '\n';
            output_hex(i);
            std::cout << ": ";
        }
        output_hex(data_segment[i++]);
        std::cout << ' ';
    }
    return 0;
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
        for (; it != end; ++it) {
            ss << it->str() << '\n';
        }
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
    if (input >> addr >> instr) {
        std::cout << addr << ' ' << instr << ' ';
        output_hex(static_cast<uint32_t>(std::stoll(instr, nullptr, 16)));
        text_segment.push_back(static_cast<uint32_t>(std::stoll(instr, nullptr, 16)));
        std::cout << '\n';
    }
}

void Linker::read_data_line(std::istream& input)
{
    std::string addr;
    std::string byte0, byte1, byte2, byte3;
    if (input >> addr >> byte0 >> byte1 >> byte2 >> byte3) {
        std::cout << addr << ' ' << byte0 << ' ' << byte1 << ' ' << byte2 << ' ' << byte3 << ' ';
        output_hex(static_cast<uint8_t>(std::stoll(byte0, nullptr, 16)));
        data_segment.push_back(static_cast<uint8_t>(std::stoll(byte0, nullptr, 16)));
        data_segment.push_back(static_cast<uint8_t>(std::stoll(byte1, nullptr, 16)));
        data_segment.push_back(static_cast<uint8_t>(std::stoll(byte2, nullptr, 16)));
        data_segment.push_back(static_cast<uint8_t>(std::stoll(byte3, nullptr, 16)));
        std::cout << ' ';
        output_hex(static_cast<uint8_t>(std::stoll(byte1, nullptr, 16)));
        std::cout << ' ';
        output_hex(static_cast<uint8_t>(std::stoll(byte2, nullptr, 16)));
        std::cout << ' ';
        output_hex(static_cast<uint8_t>(std::stoll(byte3, nullptr, 16)));
        std::cout << '\n';
    }
}

void Linker::read_symbol_line(std::istream& input)
{
    std::string address, segment, visibility, label;
    input >> address >> segment >> visibility >> label;
    uint32_t addr = std::stoul(address, nullptr, 16);
    addr += segment == "text" ? text_offset : data_offset;
    std::cout << ' ' << label << " -> " << addr << '\n';
    bool unique = segment == "text"
                  ? text_labels.insert({label, addr}).second
                  : data_labels.insert({label, addr}).second;
    if (!unique) {
        std::cerr << "duplicate symbols\n";
    }
}

void Linker::read_relocation_line(std::istream& input)
{
    std::string address, segment, visibility, instruction, label;
    input >> address >> segment >> visibility >> instruction >> label;

    uint32_t addr = std::stoul(address, nullptr, 16);
    addr += segment == "text" ? text_offset : data_offset;

    // Absolute Address
    if (instruction == "j" || instruction == "jal") {
        std::cout << "instruction " << instruction << " at " << addr << " depends on " << label << '\n';
        relocation_info.push_back(std::make_tuple(addr, instruction, label));
    }
        // External reference
    else if (instruction == "lui" || instruction == "ori") {
        std::cout << "instruction " << instruction << " at " << addr << " depends on " << label << '\n';
        relocation_info.push_back(std::make_tuple(addr, instruction, label));

    }
}
