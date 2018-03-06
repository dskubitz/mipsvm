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
    ~Assembler();

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

    Token consume(char tag, const std::string& msg);

    Token consume(Tag tag, const std::string& msg);

    [[noreturn]] void error(const std::string& msg);
    [[noreturn]] void error(Token token, const std::string& msg);

    std::string get_str(Token tok);

    int get_num(Token tok);

    void parse_line();
    void parse_label();
    void parse_instruction();
    void parse_directive();

    void print_token(Token token);
    void write_Rtype();
    void write_itype();
};


#endif //MIPS_ASSEMBLER_H
