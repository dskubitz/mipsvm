#ifndef BITMASK_H
#define BITMASK_H

template<int N>
struct Bitmask : std::integral_constant<
        decltype(~(static_cast<size_t>(~0) << N)),
        ~(static_cast<size_t>(~0) << N)> {
};

template<>
struct Bitmask<64> : std::integral_constant<size_t, ~0> {
};

#endif //BITMASK_H
