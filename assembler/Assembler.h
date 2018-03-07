#ifndef MIPS_ASSEMBLER_H
#define MIPS_ASSEMBLER_H

#include <vector>
#include "Lexer.h"

struct Parse_error : std::runtime_error {
    using std::runtime_error::runtime_error;
    using std::runtime_error::what;
};


class Assembler {
public:
    explicit Assembler(Lexer& lexer);

    int assemble();
private:
    Lexer& lex;
    Token lookahead;
    std::unordered_map<std::string, uint32_t> relocation_table;
    enum class Segment { Data, Text, };
    Segment current_segment {Segment::Text};
    std::vector<std::string> exceptions;
    static const std::unordered_map<std::string, void (Assembler::*)()> instructions;

    Token advance();

    bool match(Tag tag);

    bool match(char tag);
    template<typename T, typename... Ts>
        bool match(T tag, Ts... rest);

    Token consume(Tag tag);

    [[noreturn]] void error(Tag expected);

    const std::string& get_str(Token tok);

    int get_num(Token tok);

    void parse_line();
    void parse_label();
    void parse_instruction();
    void parse_rtype(Funct funct);
    void write_rtype(unsigned int dest, unsigned int source, unsigned int rd, unsigned int shamt,
            Funct funct);
    void parse_itype(Opcode opcode);
    void parse_jtype(Opcode opcode);
    void parse_directive();

    void print_token(Token token);
};


#endif //MIPS_ASSEMBLER_H
