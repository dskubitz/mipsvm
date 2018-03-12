#include <iostream>
#include <fstream>
#include <sstream>
#include "VM.h"

#include <Form.h>
#include <iomanip>

Form hex8(6, std::ios_base::hex, 8, '0');

size_t read_size(std::istream& input)
{
    std::string str1, str2;
    if (input >> str1 >> str2) {
        return std::stoull(str2, nullptr, 16);
    } else {
        throw std::runtime_error("Couldn't parse size\n");
    }
}

Memory load_program(std::istream& input)
{
    Memory memory;

    std::string line;
    std::getline(input, line);
    std::stringstream ss(line);
    memory.text_segment = read_size(ss) >> 2;
    memory.data_segment = read_size(ss) >> 2;
    memory.program_break = read_size(ss) >> 2;
    memory.stack_segment = 2 * memory.program_break;
    memory.reserve(memory.stack_segment);
    for (std::string line; std::getline(input, line);)
        memory.push_back(static_cast<uint32_t>(std::stoul(line, nullptr, 16)));
    memory.resize(memory.stack_segment);
    return memory;
}

VM::VM(std::istream& input)
        : registers(), memory(load_program(input)), program_counter(memory.text_segment)
{
    registers[Register::SP] = memory.stack_segment - 1;
}

int VM::execute()
{
    while (program_counter < memory.data_segment) {
        std::cout << program_counter << ' ';
        inst_t inst = memory[program_counter++];

        switch (get_opcode(inst)) {
        case Opcode::R_TYPE:
            switch (get_funct(inst)) {
            case Funct::SLL:
                std::cout << "sll " << rd(inst) << ' ' << rs(inst) << ' ' << shift_amount(inst) << '\n';
                destination_reg(inst) = target_reg(inst) << shift_amount(inst);
                break;
            case Funct::SRL:
                std::cout << "srl " << rd(inst) << ' ' << rs(inst) << ' ' << shift_amount(inst) << '\n';
                destination_reg(inst) = target_reg(inst) >> shift_amount(inst);
                break;
            case Funct::SRA:
                std::cout << "sra " << rd(inst) << ' ' << rs(inst) << ' ' << shift_amount(inst) << '\n';
                destination_reg(inst) = static_cast<int>(target_reg(inst)) >> shift_amount(inst);
                break;
            case Funct::SLLV:
                std::cout << "sra " << rd(inst) << ' ' << rs(inst) << ' ' << rt(inst) << '\n';
                destination_reg(inst) = source_reg(inst) << target_reg(inst);
                break;
            case Funct::SRLV:
                std::cout << "srlv " << rd(inst) << ' ' << rs(inst) << ' ' << rt(inst) << '\n';
                destination_reg(inst) = source_reg(inst) >> target_reg(inst);
                break;
            case Funct::SRAV:
                std::cout << "srav " << rd(inst) << ' ' << rs(inst) << ' ' << rt(inst) << '\n';
                destination_reg(inst) = static_cast<int>(source_reg(inst)) >> target_reg(inst);
                break;
            case Funct::JR:
                std::cout << "jr " << rs(inst) << '\n';
                program_counter = source_reg(inst);
                break;
            case Funct::JALR:
                destination_reg(inst) = program_counter;
                program_counter = source_reg(inst);
                break;
            case Funct::SYSCALL:
                std::cout << "syscall ";
                std::cout << "$v0 = " << registers[Register::V0].word << '\n';
                switch (registers[Register::V0].word) {
                case 1: {
                    uint32_t value = registers[Register::A0].word;
                    std::cout << value;
                    getchar();
                    break;
                }
                case 4: {
                    mem_t index = registers[Register::A0].word >> 2;
                    mem_t* address = &memory[index.word];
                    uint8_t ch;
                    while (true) {
                        ch = address->byte[0];
                        if (!ch) break;
                        putchar(ch);
                        ch = address->byte[1];
                        if (!ch) break;
                        putchar(ch);
                        ch = address->byte[2];
                        if (!ch) break;
                        putchar(ch);
                        ch = address->byte[3];
                        if (!ch) break;
                        putchar(ch);
                        ++address;
                    }
                    getchar();
                    break;
                }
                case 5: {
                    uint32_t& value = registers[Register::V0].word;
                    std::cin >> value;
                    getchar();
                    break;
                }
                case 10:
                    getchar();
                    return 0;
                case 11: {
                    uint32_t value = registers[Register::A0].word;
                    std::cout << static_cast<char>(value);
                    getchar();
                    break;
                }
                case 12: {
                    uint8_t& value = registers[Register::V0].byte[0];
                    std::cin >> value;
                    getchar();
                    break;
                }

                case 17: {
                    uint32_t value = registers[Register::A0].word;
                    getchar();
                    return value;
                }

                }
                break;
            case Funct::MFHI:
                std::cout << "mfhi " << rd(inst) << '\n';
                destination_reg(inst) = hi;
                break;
            case Funct::MTHI:
                std::cout << "mthi " << rs(inst) << '\n';
                hi = source_reg(inst);
                break;
            case Funct::MFLO:
                std::cout << "mflo " << rd(inst) << '\n';
                destination_reg(inst) = lo;
                break;
            case Funct::MTLO:
                std::cout << "mthi " << rs(inst) << '\n';
                lo = source_reg(inst);
                break;
            case Funct::MULT:
            case Funct::MULTU: {
                std::cout << "mult " << rs(inst) << ' ' << rt(inst) << '\n';
                int64_t result = static_cast<int>(source_reg(inst)) * static_cast<int>(target_reg(inst));
                hi = static_cast<uint32_t>((result & Bitmask<32>::value) >> 32);
                lo = static_cast<uint32_t>((result & (Bitmask<32>::value << 32)));
                break;
            }
            case Funct::DIV:
            case Funct::DIVU: {
                std::cout << "div " << rs(inst) << ' ' << rt(inst) << '\n';
                int64_t result = static_cast<int>(source_reg(inst)) / static_cast<int>(target_reg(inst));
                hi = static_cast<uint32_t>((result & Bitmask<32>::value) >> 32);
                lo = static_cast<uint32_t>((result & (Bitmask<32>::value << 32)));
                break;
            }
            case Funct::ADD:
            case Funct::ADDU:
                std::cout << "add " << rd(inst) << ' ' << rs(inst) << ' ' << rt(inst) << '\n';
                destination_reg(inst) = source_reg(inst) + target_reg(inst);
                break;
            case Funct::SUB:
            case Funct::SUBU:
                std::cout << "sub " << rd(inst) << ' ' << rs(inst) << ' ' << rt(inst) << '\n';
                destination_reg(inst) = source_reg(inst) - target_reg(inst);
                break;
            case Funct::AND:
                std::cout << "and " << rd(inst) << ' ' << rs(inst) << ' ' << rt(inst) << '\n';
                destination_reg(inst) = source_reg(inst) & target_reg(inst);
                break;
            case Funct::OR:
                std::cout << "or " << rd(inst) << ' ' << rs(inst) << ' ' << rt(inst) << '\n';
                destination_reg(inst) = source_reg(inst) | target_reg(inst);
                break;
            case Funct::XOR:
                std::cout << "xor " << rd(inst) << ' ' << rs(inst) << ' ' << rt(inst) << '\n';
                destination_reg(inst) = source_reg(inst) ^ target_reg(inst);
                break;
            case Funct::NOR:
                std::cout << "nor " << rd(inst) << ' ' << rs(inst) << ' ' << rt(inst) << '\n';
                destination_reg(inst) = !(source_reg(inst) | target_reg(inst));
                break;
            case Funct::SLT:
            case Funct::SLTU:
                std::cout << "slt " << rd(inst) << ' ' << rs(inst) << ' ' << rt(inst) << '\n';
                destination_reg(inst) = source_reg(inst) < target_reg(inst);
                break;
            default:
                throw std::runtime_error("Unsupported r-type operation at " + std::to_string(program_counter - 1));
            }
            break;
        case Opcode::J:
            std::cout << "j " << get_address(inst) << '\n';
            if (get_address(inst) >= memory.stack_segment) {
                std::cerr << "jump out of bounds \n";
                exit(2);
            }
            program_counter = get_address(inst);
            break;
        case Opcode::JAL:
            std::cout << "jal " << get_address(inst) << '\n';
            registers[31] = program_counter;
            program_counter = get_address(inst);
            break;
        case Opcode::BEQ:
            std::cout << "beq " << rt(inst) << ' ' << rs(inst) << ' ' << signed_extended_immediate(inst) << '\n';
            if (source_reg(inst) == target_reg(inst))
                program_counter += signed_extended_immediate(inst);
            break;
        case Opcode::BNE:
            std::cout << "bne " << rt(inst) << ' ' << rs(inst) << ' ' << signed_extended_immediate(inst) << '\n';
            if (source_reg(inst) != target_reg(inst))
                program_counter += signed_extended_immediate(inst);
            break;
        case Opcode::ADDI:
        case Opcode::ADDIU:
            std::cout << "addi " << rt(inst) << ' ' << rs(inst) << ' ' << signed_extended_immediate(inst) << '\n';
            load_target(inst) = source_reg(inst) + signed_extended_immediate(inst);
            break;
        case Opcode::SLTI:
        case Opcode::SLTIU:
            std::cout << "slti " << rt(inst) << ' ' << rs(inst) << ' ' << signed_extended_immediate(inst) << '\n';
            load_target(inst) = source_reg(inst) < signed_extended_immediate(inst);
            break;
        case Opcode::ANDI:
            std::cout << "andi " << rt(inst) << ' ' << rs(inst) << ' ' << signed_extended_immediate(inst) << '\n';
            load_target(inst) = source_reg(inst) & zero_extended_immediate(inst);
            break;
        case Opcode::ORI:
            std::cout << "ori " << rt(inst) << ' ' << rs(inst) << ' ' << signed_extended_immediate(inst) << '\n';
            load_target(inst) = source_reg(inst) | zero_extended_immediate(inst);
            break;
        case Opcode::XORI:
            std::cout << "xori " << rt(inst) << ' ' << rs(inst) << ' ' << signed_extended_immediate(inst) << '\n';
            load_target(inst) = source_reg(inst) ^ zero_extended_immediate(inst);
            break;
        case Opcode::LUI:
            std::cout << "lui " << rt(inst) << ' ' << rs(inst) << ' ' << signed_extended_immediate(inst) << '\n';
            load_target(inst) = zero_extended_immediate(inst) << 16;
            break;
        case Opcode::LW:
            std::cout << "lw " << rt(inst) << ' ' << rs(inst) << ' ' << signed_extended_immediate(inst) << '\n';
            load_target(inst) = memory[(source_reg(inst) + signed_extended_immediate(inst)) >> 2];
            break;
        case Opcode::SW:
            std::cout << "sw " << rt(inst) << ' ' << rs(inst) << ' ' << signed_extended_immediate(inst) << '\n';
            memory[(source_reg(inst) + signed_extended_immediate(inst)) >> 2] = target_reg(inst);
            break;
        default:
            throw std::runtime_error("Unsupported i-type operation at " + std::to_string(program_counter - 1));
        }
        std::cout << registers;
        getchar();
    }
    return 0;
}

int main(int argc, char** argv)
{
    if (argc != 2) {
        std::cerr << "usage: " << argv[0] << " file\n";
        exit(1);
    }
    std::ifstream input(argv[1]);
    if (!input.is_open()) {
        std::cerr << "Couldn't open file.\n";
        exit(1);
    }
    VM vm(input);
    return vm.execute();
}

std::ostream& operator<<(std::ostream& os, const mem_t& mem)
{
    return os << mem.word;
}

std::ostream& operator<<(std::ostream& os, const RegisterFile& regfile)
{
    for (size_t i = 0; i < 32;) {
        os << std::setw(5) << std::left << static_cast<Register>(i) << " " << hex8(regfile[i]) << "   ";
        i += 1;
        if (i % 8 == 0)
            os << '\n';
    }
    return os;
}
