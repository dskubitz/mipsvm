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

#include <Register.h>
#include <Opcode.h>
#include <Funct.h>

enum class Tag : int {
    Identifier=256,
    Label,

    String,
    Immediate,

    Directive,

    Instruction,
    Register,

    COLON=':',
    MINUS='-',
    NL='\n',
    Eof = EOF,
};

extern const std::unordered_map<std::string, Tag> reserved_words;

enum class Directive {
    ALIGN, ASCII, ASCIIZ, BYTE, DATA, EXTERN, GLOBL, HALF, SPACE, TEXT, WORD,
};

extern const std::unordered_map<std::string, Directive> directives;

template<class Enum>
auto as_integer(Enum value)
{
    return static_cast<typename std::underlying_type<Enum>::type>(value);
}

std::ostream& operator<<(std::ostream& os, const Tag& tag);

struct SourceLocation {
    unsigned int line;
    unsigned int column;

    SourceLocation(unsigned int l = 0, unsigned int c = 0)
            :line(l),
             column(c)
    { }

    SourceLocation(const SourceLocation& loc)
            :line(loc.line),
             column(loc.column)
    {
    }

    SourceLocation& operator=(const SourceLocation& loc)
    {
        if (this != &loc) {
            SourceLocation tmp(loc);
            swap(tmp);
        }
        return *this;
    }

    SourceLocation(SourceLocation&& loc) noexcept
            :line(loc.line),
             column(loc.column)
    {
    }

    SourceLocation& operator=(SourceLocation&& loc)
    {
        swap(loc);
        return *this;
    }

    void swap(SourceLocation& loc)
    {
        using std::swap;
        swap(line, loc.line);
        swap(column, loc.column);
    }

    std::string to_string() const;
};

inline void swap(SourceLocation& lhs, SourceLocation& rhs)
{
    lhs.swap(rhs);
}

inline bool operator==(const SourceLocation& lhs, const SourceLocation& rhs)
{
    return std::tie(lhs.line, lhs.column) == std::tie(rhs.line, rhs.column);
}

inline bool operator!=(const SourceLocation& lhs, const SourceLocation& rhs)
{
    return !(rhs == lhs);
}

inline bool operator<(const SourceLocation& lhs, const SourceLocation& rhs)
{
    return std::tie(lhs.line, lhs.column) < std::tie(rhs.line, rhs.column);
}

inline bool operator>(const SourceLocation& lhs, const SourceLocation& rhs)
{
    return rhs < lhs;
}

inline bool operator<=(const SourceLocation& lhs, const SourceLocation& rhs)
{
    return !(rhs < lhs);
}

inline bool operator>=(const SourceLocation& lhs, const SourceLocation& rhs)
{
    return !(lhs < rhs);
}

std::ostream& operator<<(std::ostream& os, const SourceLocation& location);

namespace std {
template<>
struct hash<SourceLocation> {
    constexpr size_t operator()(const SourceLocation& loc) const noexcept
    {
        return (static_cast<size_t>(loc.column) << 32) | static_cast<size_t>(loc.line);
    }
};
} //namespace std

class Token {
public:
    explicit Token(Tag tag = Tag::Eof, SourceLocation loc = {})
            :tag_(tag),
             location_(loc)
    {
    }

    explicit Token(char tag, SourceLocation loc = {})
            :Token(static_cast<Tag>(tag), loc)
    {
    }

    Token(const Token& token) = default;

    Tag tag() const noexcept
    { return tag_; }

    const SourceLocation& location() const noexcept
    { return location_; }

private:
    Tag tag_;
    SourceLocation location_;
};


std::ostream& operator<<(std::ostream& os, const Token& token);

#endif //DRAGON_TOKEN_H
