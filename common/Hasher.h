#ifndef HASHER_H
#define HASHER_H

#include <string>
#include <algorithm>

struct case_insensitive_hash {
    using result_type = size_t;
    using argument_type = std::string;

    result_type operator()(const argument_type& str) const
    {
        result_type h = 0;
        for (const auto& x : str) {
            h <<= 1;
            h ^= toupper(x);
        }
        return h;
    }
};

struct case_insensitive_equal {
    using argument_type = std::string;

    bool operator()(const argument_type& lhs, const argument_type& rhs) const
    {
        if (lhs.size() != rhs.size())
            return false;
        for (size_t i = 0; i < lhs.size(); ++i) {
            if (toupper(lhs[i]) != toupper(rhs[i]))
                return false;
        }
        return true;
    }
};

#endif //MIPS_HASHER_H
