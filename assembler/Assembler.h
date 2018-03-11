#ifndef MIPS_ASSEMBLER_H
#define MIPS_ASSEMBLER_H

#include <iostream>
#include <iomanip>
#include <vector>
#include <sstream>

#include <boost/variant/get.hpp>
#include <boost/type_index.hpp>
#include "Lexer.h"
#include <Bitmask.h>

struct Parse_error : std::runtime_error {
    using std::runtime_error::runtime_error;
    using std::runtime_error::what;
};

enum class Segment : bool {
    Text, Data,
};

enum class Visibility : bool {
    Local, Global,
};

inline std::ostream& operator<<(std::ostream& os, Segment segment)
{
    switch (segment) {
    case Segment::Data:
        return os << "data";
    case Segment::Text:
        return os << "text";
    }
}

inline std::ostream& operator<<(std::ostream& os, Visibility visibility)
{
    switch (visibility) {
    case Visibility::Local:
        return os << "local";
    case Visibility::Global:
        return os << "global";
    }
}

// @formatter:off
//template<class ... T>
//struct all_integral : std::is_integral<typename std::common_type<T...>::type> { };

struct address_type {
    uint32_t addr;
    Visibility vis;
    Segment seg;

    address_type() = default;
    address_type(const address_type&) = default;
    address_type& operator=(const address_type&) = default;

    address_type(uint32_t a, Visibility v = {}, Segment s = {}) :addr(a), vis(v), seg(s) { } // NOLINT

    address_type& operator=(uint32_t address) { addr = address; return *this; }

    friend bool operator==(const address_type& lhs, const address_type& rhs)
    { return std::tie(lhs.addr, lhs.vis, lhs.seg) == std::tie(rhs.addr, rhs.vis, rhs.seg); }

    friend bool operator!=(const address_type& lhs, const address_type& rhs)
    { return !(rhs == lhs); }
};

inline std::ostream& operator<<(std::ostream& os, const address_type& address)
{
    return os << std::hex << std::setw(8) << std::setfill('0') << address.addr
              << std::dec << std::setfill(' ') << ' ' << address.seg << ' ' << address.vis;
}

struct reloc_info {
    Opcode instr_type;
    std::reference_wrapper<const std::string> depencency;

    reloc_info(Opcode o = {}, const std::string& str = {}) // NOLINT
            :instr_type(o), depencency(std::ref<const std::string>(str)) { }
};

inline std::ostream& operator<<(std::ostream& os, const reloc_info& info)
{
    return os << info.instr_type << ' ' << info.depencency.get() ;
}

// @formatter:on

namespace std {
template<>
struct hash<address_type> {
    size_t operator()(const address_type& a) const noexcept
    {
        return hash<uint32_t>{}(a.addr);
    }
};
} //namespace std

class Assembler {
public:
    explicit Assembler(Lexer& lexer);

    int assemble(std::ostream& output);

private:
    Lexer& lex;
    Token lookahead;

    Segment current_segment {Segment::Text};

    // Absolute address labels go here
    std::unordered_map<address_type, reloc_info> relocation_table;

    // Relative address labels go here
    std::unordered_map<std::string, address_type> symbol_table;

    // branch labels -> branch instruction addresses
    std::unordered_map<std::string, address_type> branch_labels;

    // Which labels have been declared .globl
    std::vector<std::string> globals;

    std::unordered_map<uint32_t, std::reference_wrapper<const std::string>> addr_to_label;

    std::vector<uint32_t> text_segment;
    std::vector<std::string> instructions;
    std::vector<uint8_t> data_segment;
    uint32_t text_address {0};
    uint32_t data_address {0};
    uint8_t data_alignment {4};
    bool error_occured {false};

    void synchronize()
    {
        error_occured = true;
        while (!match(Tag::Directive, Tag::Label, Tag::Instruction, Tag::Eof)) {
            advance();
        }
    }

    address_type current_address()
    {
        if (current_segment == Segment::Text)
            return {text_address, Visibility::Local, current_segment};
        else
            return {data_address, Visibility::Local, current_segment};
    }

    Token advance();
    bool match(Tag tag);
    template<typename T, typename... Ts>
    bool match(T tag, Ts... rest);
    Token consume(Tag tag);
    [[noreturn]] void error(Tag expected);

    template<class T>
    const T& get(Token tok)
    {
        static T bad; // So clang wouldn't complain
        try {
            return boost::get<T>(lex.table().at(tok.location()));
        }
        catch (std::exception& e) {
            std::cerr << e.what()
                      << ": caught when reading a(n)"
                      << boost::typeindex::type_id<T>().pretty_name()
                      << " at " << tok.location() << '\n';
            synchronize();
        }
        return bad;
    }

    void print_data_segment();
    void backpatch_references();
    void output_object(std::ostream& output);

    void parse_line();
    void parse_label();
    void parse_instruction();
    void parse_directive();

    void parse_rtype(Funct funct);
    void parse_itype(Opcode opcode);
    void parse_jtype(Opcode opcode);

    template<class... Ts>
    std::string string_inst(Ts... args)
    {
        std::stringstream ss;
        string_inst_impl(ss, args...);
        return ss.str();
    }

    template<class T>
    void string_inst_impl(std::ostream& os, T arg)
    {
        os << arg;
    }

    template<class T, class ...Ts>
    void string_inst_impl(std::ostream& os, T arg, Ts... args)
    {
        os << arg;
        string_inst_impl(os, args...);

    }

    void write_rtype(Register dest, Register source, Register target, unsigned shamt, Funct funct)
    {
        uint32_t inst = ((as_integer(source) & Bitmask<5>::value) << 21)
                        | ((as_integer(target) & Bitmask<5>::value) << 16)
                        | ((as_integer(dest) & Bitmask<5>::value) << 11)
                        | ((shamt & Bitmask<5>::value) << 6)
                        | (as_integer(funct) & Bitmask<6>::value);

        text_segment.push_back(inst);
        text_address += 4;
    }

    void write_itype(Opcode opcode, Register dest, Register source, unsigned immediate)
    {
        uint32_t inst = ((as_integer(opcode) & Bitmask<6>::value) << 26)
                        | ((as_integer(source) & Bitmask<5>::value) << 21)
                        | ((as_integer(dest) & Bitmask<5>::value) << 16)
                        | (immediate & Bitmask<16>::value);

        text_segment.push_back(inst);
        text_address += 4;
    }

    void write_jtype(Opcode opcode, int addr)
    {
        uint32_t inst = ((as_integer(opcode) & Bitmask<6>::value) << 26)
                        | (addr & Bitmask<26>::value);

        text_segment.push_back(inst);
        text_address += 4;
    }

    void pad_remaining(int written);
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

#endif //MIPS_ASSEMBLER_H
