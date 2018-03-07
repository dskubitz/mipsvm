#include "Lexer.h"
#include <sstream>
#include <algorithm>

namespace {
int escaped_char(int ch)
{
    switch (ch) {
    case 'n':
        return '\n';
    case 't':
        return '\t';
    case '\\':
        return '\\';
    case '\"':
        return '\"';
    case '0':
        return '\0';
    default:
        return ch;
    }
}

bool isalnum_(int ch)
{
    return isalnum(ch) || ch == '_';
}

/*
void stolower(std::string& str)
{
    std::transform(str.begin(), str.end(), str.begin(),
                   [](const auto& a) { return ::tolower(a); });
}
*/
}

Lexer::Lexer(std::istream& input)
        :input_(input),
         line_(1),
         column_(0)
{ }

Token Lexer::scan()
{
    int ch = advance();
    while (ch != EOF && isspace(ch)) {
        ch = advance();
    }
    location_ = {line_, column_};
    switch (ch) {
    case '#':
        return comment(ch);
    case '"':
        return scan_string(ch);
    case '.':
        return scan_directive(ch);
    case ',':
        return scan();
    case '(':
        return scan();
    case ')':
        return scan();
    default:
        if (ch == '_' || isalpha(ch) || ch == '$')
            return scan_keyword_or_identifier(ch);
        else if (isdigit(ch))
            return scan_number(ch);
        else
            return make_token(ch);
    }
}

Token Lexer::scan_number(int ch)
{
    std::string lexeme(1, static_cast<char>(ch));

    while (isdigit(peek()))
        lexeme.push_back(static_cast<char>(advance()));
    table_[location_] = std::stoi(lexeme);

    return make_token(Tag::Immediate);
}

Token Lexer::scan_directive(int ch)
{
    std::string lexeme(1, static_cast<char>(ch));
    while (isalnum_(peek()))
        lexeme.push_back(static_cast<char>(ch = advance()));

    auto it = reserved_words.find(lexeme);
    if (it == reserved_words.end())
        error("malformed directive");

    table_[location_] = lexeme;
    return make_token(it->second);
}

Token Lexer::scan_keyword_or_identifier(int ch)
{
    std::string lexeme(1, static_cast<char>(ch));

    while (isalnum_(peek()))
        lexeme.push_back(static_cast<char>(ch = advance()));

    auto word = reserved_words.find(lexeme);

    if (word != reserved_words.end()) {
        auto regit = registers.find(lexeme);
        if (regit != registers.end()) {
            table_[location_] = as_integer(regit->second);
        }
        else {
            table_[location_] = lexeme;
        }
        return make_token(word->second);
    }
    else {
        if (peek() == ':') {
            advance();
            table_[location_] = lexeme;
            return make_token(Tag::Label);
        }
        table_[location_] = lexeme;
        return make_token(Tag::Identifier);
    }
}

Token Lexer::comment(int ch)
{
    do {
        ch = advance();
    }
    while (ch != '\n' && ch != EOF);
    return scan();
}

Token Lexer::scan_string(int ch)
{
    std::string lexeme;
    for (;;) {
        ch = advance();
        switch (ch) {
        case '\\':
            lexeme.push_back(static_cast<char>(escaped_char(advance())));
            continue;
        case '\"':
            break;
        default:
            lexeme.push_back(static_cast<char>(ch));
            continue;
        }
        break;
    }
    table_[location_] = lexeme;
    return make_token(Tag::String);
}


/*
bool Lexer::match(int ch)
{
    if (peek() != ch)
        return false;
    advance();
    return true;
}
*/

int Lexer::peek()
{
    return input_.peek();
}

int Lexer::advance()
{
    int ch = input_.get();
    ++column_;
    if (ch == '\n') {
        ++line_;
        column_ = 0;
    }
    return ch;
}
