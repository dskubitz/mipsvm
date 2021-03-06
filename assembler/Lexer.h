//
// Created by dvsku on 2/20/2018.
//

#ifndef LEXER_H
#define LEXER_H

#include <iosfwd>
#include <string>
#include <memory>
#include <unordered_map>
#include <unordered_set>

#include "Token.h"
#include <boost/variant/variant.hpp>

struct Lexer_error : public std::runtime_error {
    using std::runtime_error::runtime_error;
    using std::runtime_error::what;
};

class Lexer {
public:
    using Variant = boost::variant<int, std::string, Register>;

    explicit Lexer(std::istream& input);
    Token scan();

    const std::unordered_map<SourceLocation, Variant>& table() const noexcept
    { return table_; }

private:
    std::unordered_map<SourceLocation, Variant> table_;
    std::istream& input_;
    unsigned int line_;
    unsigned int column_;
    SourceLocation location_;

    Token scan_number(int ch);
    Token scan_directive(int ch);
    Token scan_keyword_or_identifier(int ch);
    Token scan_string(int ch);
    Token comment(int ch);
//    bool match(int ch);
    int peek();
    int advance();

    [[noreturn]] void error(const std::string& msg);

    Token make_token(char ch);

    Token make_token(Tag tag);
};

inline void Lexer::error(const std::string& msg)
{
    throw Lexer_error(
            location_.to_string() + ": error: " + msg
    );
}

inline Token Lexer::make_token(char ch)
{
    return Token(ch, location_);
}

inline Token Lexer::make_token(Tag tag)
{
    return Token(tag, location_);
}

#endif //DRAGON_LEXER_H
