#ifndef MIPS_ASSEMBLER_H
#define MIPS_ASSEMBLER_H

#include <vector>
#include "Lexer.h"

struct Parse_error : std::runtime_error {
    using std::runtime_error::runtime_error;
    using std::runtime_error::what;
};

enum class Segment : uint8_t { Data, Text, };
inline std::ostream& operator<<(std::ostream& os, Segment segment)
{
    switch (segment) {
    case Segment::Data:
        return os << "data";
    case Segment::Text:
        return os << "text";
    }
}

// @formatter:off
template<class ... T>
struct all_integral : std::is_integral<typename std::common_type<T...>::type> { };

struct address_type {
    uint32_t addr;
    Segment seg;

    address_type() = default;
    address_type(const address_type&) = default;
    address_type& operator=(const address_type&) = default;

    address_type(uint32_t a, Segment s = {}) :addr(a), seg(s) { } // NOLINT

    address_type& operator=(uint32_t address) { addr = address; return *this; }

    friend bool operator==(const address_type& lhs, const address_type& rhs)
    { return std::tie(lhs.addr, lhs.seg)==std::tie(rhs.addr, rhs.seg); }

    friend bool operator!=(const address_type& lhs, const address_type& rhs)
    { return !(rhs==lhs); }
};
// @formatter:on

namespace std {
template<>
struct hash<address_type> {
    size_t operator()(const address_type& a) const noexcept
    {
        return hash<uint32_t>{}(a.addr) ^ hash<uint8_t>{}(as_integer(a.seg));
    }
};
} //namespace std

class Assembler {
public:
    explicit Assembler(Lexer& lexer);

    int assemble();

private:
    Lexer& lex;
    Token lookahead;

    Segment current_segment{Segment::Text};

    // Absolute address labels go here
    // Only subroutine call and load and store instructions reference absolute addresses
    std::unordered_map<std::string, address_type> relocation_table;

    // jump labels -> jump instruction addresses
    // These should look for entries in the relocation_table
    std::unordered_map<std::string, address_type> jump_labels;

    // Relative address labels go here
    std::unordered_map<std::string, address_type> symbol_table;

    // branch labels -> branch instruction addresses
    std::unordered_map<std::string, address_type> branch_labels;

    // Which labels have been declared .globl
    std::unordered_set<std::string> globals;

    std::vector<uint32_t> text_segment;
    std::vector<uint8_t> data_segment;
    uint32_t text_address{0};
    uint32_t data_address{0};
    uint8_t data_alignment{4};

    // Debugging
    std::vector<std::string> exceptions;

    address_type current_address()
    {
        if (current_segment==Segment::Text)
            return {text_address, current_segment};
        else
            return {data_address, current_segment};
    }

    Token advance();
    bool match(Tag tag);
    template<typename T, typename... Ts>
    bool match(T tag, Ts... rest);
    Token consume(Tag tag);
    [[noreturn]] void error(Tag expected);

    const std::string& get_str(Token tok);
    int get_num(Token tok);
    unsigned get_unsigned(Token tok);

    void print_data_segment();

    void parse_line();
    void parse_label();
    void parse_instruction();
    void parse_directive();

    void parse_rtype(Funct funct);
    void parse_itype(Opcode opcode);
    void parse_jtype(Opcode opcode);

    template<class T1, class T2, class T3, class T4>
    std::enable_if_t<all_integral<T1, T2, T3, T4>::value>
    write_rtype(T1 dest, T2 source, T3 target, T4 shamt, Funct funct)
    {
        uint32_t inst = ((source & 31) << 21) | ((target & 31) << 16)
                | ((dest & 31) << 11) | ((shamt & 31) << 6)
                | (as_integer(funct) & 63);

        text_segment.push_back(inst);
        text_address += 4;
    }

    template<class T1, class T2, class T3>
    std::enable_if_t<all_integral<T1, T2, T3>::value>
    write_itype(Opcode opcode, T1 dest, T2 source, T3 immediate)
    {
        uint32_t inst = ((as_integer(opcode) & 63) << 26) | ((source & 31) << 21)
                | ((dest & 31) << 16) | (immediate & 65535);

        text_segment.push_back(inst);
        text_address += 4;
    }

    template<class T>
    std::enable_if_t<all_integral<T>::value>
    write_jtype(Opcode opcode, T addr)
    {
        uint32_t inst = ((as_integer(opcode) & 63) << 26) | (addr & 67108863U);

        text_segment.push_back(inst);
        text_address += 4;
    }
};

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

inline unsigned Assembler::get_unsigned(Token tok)
{
    return static_cast<unsigned>(get_num(tok));
}



#endif //MIPS_ASSEMBLER_H
