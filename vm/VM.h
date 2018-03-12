#ifndef MIPS_VM_H
#define MIPS_VM_H

#include <Opcode.h>
#include <Register.h>
#include <Funct.h>
#include <Bitmask.h>
#include <cstring>
#include <vector>

using inst_t = uint32_t;

union mem_t {
    mem_t(uint32_t w = 0) // NOLINT
            : word(w) { }

    uint32_t word;
    uint16_t half[2];
    uint8_t byte[4];

    operator inst_t() { return word; }

    operator int() { return static_cast<int>(word); }
};

struct RegisterFile {
    RegisterFile() { memset(reg, 0, sizeof reg); }

    mem_t reg[32];

    mem_t& operator[](std::size_t i) { return reg[i]; }

    mem_t& operator[](Register i) { return reg[static_cast<size_t>(i)]; }

    const mem_t& operator[](std::size_t i) const { return reg[i]; }

    const mem_t& operator[](Register i) const { return reg[static_cast<size_t>(i)]; }
};

std::ostream& operator<<(std::ostream& os, const RegisterFile& regfile);

std::ostream& operator<<(std::ostream& os, const mem_t& mem);

class Memory : private std::vector<mem_t> {
    using base = std::vector<mem_t>;
public:
    using base::vector;
    using base::operator[];
    using base::push_back;
    using base::resize;
    using base::reserve;
    using base::size;
    using base::capacity;
    using base::begin;
    using base::end;
    size_type text_segment{0};
    size_type data_segment{0};
    size_type program_break{0};
    size_type stack_segment{0};
};

struct VM {
    explicit VM(std::istream& input);
    int execute();

    Opcode get_opcode(inst_t instruction)
    {
        return static_cast<Opcode>((instruction & (Bitmask<6>::value << 26)) >> 26);
    }

    Funct get_funct(inst_t instruction)
    {
        return static_cast<Funct>(instruction & Bitmask<6>::value);
    }

    Register rs(inst_t inst)
    {
        return static_cast<Register>((inst & (Bitmask<5>::value << 21)) >> 21);
    }

    Register rt(inst_t inst)
    {
        return static_cast<Register>((inst & (Bitmask<5>::value << 16)) >> 16);
    }

    Register rd(inst_t inst)
    {
        return static_cast<Register>((inst & (Bitmask<5>::value << 11)) >> 11);
    }

    uint32_t source_reg(inst_t inst)
    {
        return registers[rs(inst)];
    }

    uint32_t target_reg(inst_t inst)
    {
        return registers[rt(inst)];
    }

    mem_t& load_target(inst_t inst)
    {
        return registers[rt(inst)];
    }

    mem_t& destination_reg(inst_t inst)
    {
        return registers[rd(inst)];
    }

    int shift_amount(inst_t instruction)
    {
        return (instruction & (Bitmask<5>::value << 6)) >> 6;
    }

    int signed_extended_immediate(inst_t instruction)
    {
        return static_cast<int16_t>(instruction & Bitmask<16>::value);
    }

    int zero_extended_immediate(inst_t instruction)
    {
        return instruction & Bitmask<16>::value;
    }

    int get_address(inst_t instruction)
    {
        return instruction & Bitmask<26>::value;
    }

    RegisterFile registers;
    Memory memory;
    uint32_t hi{0};
    uint32_t lo{0};
    uint32_t program_counter{0};
};

#endif //MIPS_VM_H
