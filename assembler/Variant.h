//
// Created by dvsku on 3/3/2018.
//

#ifndef DRAGON_VARIANT_H
#define DRAGON_VARIANT_H

#include <ostream>


struct Variant {
    enum class Tag {
        String, Int, Double,
    };

    Variant();

    explicit Variant(std::string s);

    explicit Variant(int i);

    explicit Variant(double d);

    ~Variant();

    Variant(const Variant& other);
    Variant& operator=(const Variant& other);

    Variant& operator=(const std::string& s);

    Variant& operator=(int i);

    Variant& operator=(double d);

    std::string& str();

    const std::string& str() const;

    int& num();

    const int& num() const;

    double& real();

    const double& real() const;

    Tag tag() const;
private:
    Tag tag_;
    union {
        std::string str_;
        int num_;
        double real_;
    };

    void check_access(Tag tag) const;
};

std::ostream& operator<<(std::ostream& os, const Variant& variant);

#endif //DRAGON_VARIANT_H
