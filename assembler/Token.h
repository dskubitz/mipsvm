//
// Created by dvsku on 2/26/2018.
//

#ifndef DRAGON_TOKEN_H
#define DRAGON_TOKEN_H

#include <string>
#include <iosfwd>
#include <tuple>
#include <type_traits>

enum class Tag : int {
    Identifier=256,
    Label,

    String,
    Immediate,

    Directive,

    Instruction,
    Register,

    New_line = '\n',
    Eof = EOF,
};

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
