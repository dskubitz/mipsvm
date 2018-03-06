//
// Created by dvsku on 2/26/2018.
//
#include "Token.h"
#include <ostream>

std::ostream& operator<<(std::ostream& os, const Source_location& location)
{
    return os << "Line " << location.line << " Column " << location.column;
}

std::ostream& operator<<(std::ostream& os, const Token& token)
{
    os << token.location();
    return os;
}

std::string Source_location::to_string() const
{
    return "Line " + std::to_string(line)
            + ", Column " + std::to_string(column);
}
