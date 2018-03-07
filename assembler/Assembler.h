#ifndef MIPS_ASSEMBLER_H
#define MIPS_ASSEMBLER_H

#include <vector>
#include "Lexer.h"

struct Parse_error : std::runtime_error {
    using std::runtime_error::runtime_error;
    using std::runtime_error::what;
};

enum class Segment : uint8_t { Data, Text, };


// @formatter:off
struct address_type {
    uint32_t addr;
    Segment seg;
    address_type() = default;
    address_type(const address_type&) = default;
    address_type& operator=(const address_type&) = default;

    address_type(uint32_t a, Segment s = {}) : addr(a), seg(s) { }

    address_type& operator=(uint32_t address) { addr = address; return *this; }

    friend bool operator==(const address_type& lhs, const address_type& rhs)
    { return std::tie(lhs.addr, lhs.seg) == std::tie(rhs.addr, rhs.seg); }

    friend bool operator!=(const address_type& lhs, const address_type& rhs)
    { return !(rhs == lhs); }
};
// @formatter:on

namespace std {
template<> struct hash<address_type> {
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

    Segment current_segment {Segment::Text};

    // Absolute address labels go here
    // Only subroutine call and load and store instructions reference absolute addresses
    std::unordered_map<std::string, address_type> relocation_table;

    // Relative address labels go here
    std::unordered_map<std::string, address_type> symbol_table;

    std::unordered_map<address_type, std::string> resolved;
    std::unordered_set<std::string> globals;

    std::vector<uint32_t> text_segment;
    std::vector<uint8_t> data_segment;
    uint32_t text_address {0};
    uint32_t data_address {0};
    uint8_t data_alignment {4};

    // Debugging
    std::vector<std::string> exceptions;

    address_type current_address()
    {
        if (current_segment==Segment::Text)
            return {text_address, current_segment};
        else
            return {data_address, current_segment};
    }

    void add_resolved(const std::string& label, address_type address)
    {
        symbol_table[label] = address;
        resolved[address] = label;
    }

    Token advance();
    bool match(Tag tag);
    template<typename T, typename... Ts> bool match(T tag, Ts... rest);
    Token consume(Tag tag);
    [[noreturn]] void error(Tag expected);
    const std::string& get_str(Token tok);
    int get_num(Token tok);
    unsigned get_unsigned(Token tok);

    void parse_line();
    void parse_label();
    void parse_instruction();
    void parse_rtype(Funct funct);
    void write_rtype(unsigned dest, unsigned source, unsigned rd, unsigned shamt,
                     Funct funct);
    void write_itype(Opcode opcode, unsigned dest, unsigned source, unsigned immediate);
    void parse_itype(Opcode opcode);
    void parse_jtype(Opcode opcode);
    void parse_directive();
};


inline Assembler::Assembler(Lexer& lexer)
        :lex(lexer), lookahead(lex.scan())
{ }

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
    return lookahead.tag() == tag;
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
