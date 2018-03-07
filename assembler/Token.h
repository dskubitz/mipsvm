//
// Created by dvsku on 2/26/2018.
//

#ifndef DRAGON_TOKEN_H
#define DRAGON_TOKEN_H

#include <string>
#include <iosfwd>
#include <tuple>
#include <type_traits>
#include <unordered_map>


enum class Tag : int {
    Identifier=256,
    Label,

    String,
    Immediate,

    Directive,

    Instruction,
    Register,

    Eof = EOF,
};

extern const std::unordered_map<std::string, Tag> reserved_words;

enum class Opcode {
    R_TYPE = 0,
    J = 2, JAL = 3,
    BEQ = 4, BNE = 5, BLEZ = 6, BGTZ = 7,
    ADDI = 8, ADDIU = 9, SLTI = 10, SLTIU = 11,
    ANDI = 12, ORI = 13, XORI = 14, LUI = 15,
    LB = 32, LH = 33, LWL = 34, LW = 35,
    LBU = 36, LHU = 37, LWR = 38,
    SB = 40, SH = 41, SWL = 42, SW = 43,
    SWR = 46, CACHE = 47,
    LL = 48, LWC1 = 49, LWC2 = 50, PREF = 51,
    LDC1 = 53, LDC2 = 54,
    SC = 56, SWC1 = 57, SWC2 = 58,
    SDC1 = 61, SDC2 = 62, LI=65, MOVE=66,
};
extern const std::unordered_map<std::string, Opcode> opcodes;

enum class Funct {
    SLL = 0, SRL = 2, SRA = 3,
    SLLV = 4, SRLV = 6, SRAV = 7,
    JR = 8, JALR = 9, MOVZ = 10, MOVN = 11,
    SYSCALL = 12, BREAK = 13, SYNC = 15,
    MFHI = 16, MTHI = 17, MFLO = 18, MTLO = 19,
    MULT = 24, MULTU = 25, DIV = 26, DIVU = 27,
    ADD = 32, ADDU = 33, SUB = 34, SUBU = 35,
    AND = 36, OR = 37, XOR = 38, NOR = 39,
    SLT = 42, SLTU = 43,
    TGE = 48, TGEU = 49, TLT = 50, TLTU = 51,
    TEQ = 52, TNE = 54,
};

extern const std::unordered_map<std::string, Funct> functs;

enum class Reg {
    ZERO = 0, AT = 1,
    V0 = 2, V1 = 3,
    A0 = 4, A1 = 5, A2 = 6, A3 = 7,
    T0 = 8, T1 = 9, T2 = 10, T3 = 11, T4 = 12, T5 = 13, T6 = 14, T7 = 15,
    S0 = 16, S1 = 17, S2 = 18, S3 = 19, S4 = 20, S5 = 21, S6 = 22, S7 = 23,
    T8 = 24, T9 = 25,
    K0 = 26, K1 = 27,
    GP = 28, SP = 29, FP = 30, RA = 31,
};

extern const std::unordered_map<std::string, Reg> registers;

template<class Enum>
auto as_integer(Enum value)
{
    return static_cast<typename std::underlying_type<Enum>::type>(value);
}

std::ostream& operator<<(std::ostream& os, const Tag& tag);


struct Source_location {
    unsigned int line;
    unsigned int column;

    Source_location(unsigned int l = 0, unsigned int c = 0)
            :line(l),
             column(c)
    { }

    Source_location(const Source_location& loc)
            :line(loc.line),
             column(loc.column)
    {
    }

    Source_location& operator=(const Source_location& loc)
    {
        if (this != &loc) {
            Source_location tmp(loc);
            swap(tmp);
        }
        return *this;
    }

    Source_location(Source_location&& loc) noexcept
            :line(loc.line),
             column(loc.column)
    {
    }

    Source_location& operator=(Source_location&& loc)
    {
        swap(loc);
        return *this;
    }

    void swap(Source_location& loc)
    {
        using std::swap;
        swap(line, loc.line);
        swap(column, loc.column);
    }

    std::string to_string() const;
};


inline void swap(Source_location& lhs, Source_location& rhs)
{
    lhs.swap(rhs);
}

inline bool operator==(const Source_location& lhs, const Source_location& rhs)
{
    return std::tie(lhs.line, lhs.column) == std::tie(rhs.line, rhs.column);
}

inline bool operator!=(const Source_location& lhs, const Source_location& rhs)
{
    return !(rhs == lhs);
}

inline bool operator<(const Source_location& lhs, const Source_location& rhs)
{
    return std::tie(lhs.line, lhs.column) < std::tie(rhs.line, rhs.column);
}

inline bool operator>(const Source_location& lhs, const Source_location& rhs)
{
    return rhs < lhs;
}

inline bool operator<=(const Source_location& lhs, const Source_location& rhs)
{
    return !(rhs < lhs);
}

inline bool operator>=(const Source_location& lhs, const Source_location& rhs)
{
    return !(lhs < rhs);
}

std::ostream& operator<<(std::ostream& os, const Source_location& location);

namespace std {
template<> struct hash<Source_location> {
    constexpr size_t operator()(const Source_location& loc) const noexcept
    {
        return (static_cast<size_t>(loc.column) << 32) | static_cast<size_t>(loc.line);
    }
};
} //namespace std

class Token {
public:
    explicit Token(Tag tag = Tag::Eof, Source_location loc = {})
            :tag_(tag),
             location_(loc)
    {
    }

    explicit Token(char tag, Source_location loc = {})
            :Token(static_cast<Tag>(tag), loc)
    {
    }

    Token(const Token& token) = default;

    Tag tag() const noexcept
    { return tag_; }

    const Source_location& location() const noexcept
    { return location_; }

private:
    Tag tag_;
    Source_location location_;
};


std::ostream& operator<<(std::ostream& os, const Token& token);

#endif //DRAGON_TOKEN_H
