//
// Created by dvsku on 3/3/2018.
//
#include <stdexcept>
#include "Variant.h"
#include <string>

Variant::Variant()
        :tag_(Tag::String), str_ {}
{ }

Variant::Variant(std::string s)
        :tag_ {Tag::String}, str_ {std::move(s)}
{ }

Variant::Variant(int i)
        :tag_ {Tag::Int}, num_(i)
{ }

Variant::Variant(double d)
        :tag_ {Tag::Double}, real_(d)
{ }

Variant::~Variant()
{
    using std::string;
    if (tag_ == Tag::String)
        str_.~string();
}

Variant::Variant(const Variant& other)
{
    using std::string;

    if (this != &other) {
        if (tag_ == Tag::String && other.tag_ == Tag::String) {
            str_ = other.str_;
        }

        if (tag_ == Tag::String) str_.~string();

        switch (other.tag_) {
        case Tag::Int:
            num_ = other.num_;
            break;
        case Tag::Double:
            real_ = other.real_;
            break;
        case Tag::String:
            new(&str_) string(other.str_);
            break;
        }
        tag_ = other.tag_;
    }
}

Variant& Variant::operator=(const std::string& s)
{
    if (tag_ == Tag::String) {
        str_ = s;
    }
    else {
        new(&str_) std::string(s);
        tag_ = Tag::String;
    }
}

Variant& Variant::operator=(int i)
{
    using std::string;
    if (tag_ == Tag::String) str_.~string();
    num_ = i;
    tag_ = Tag::Int;
}

Variant& Variant::operator=(double d)
{
    using std::string;
    if (tag_ == Tag::String) str_.~string();
    real_ = d;
    tag_ = Tag::Double;
}

std::string& Variant::str()
{
    check_access(Tag::String);
    return str_;
}

const std::string& Variant::str() const
{
    check_access(Tag::String);
    return str_;
}

int& Variant::num()
{
    check_access(Tag::Int);
    return num_;
}

const int& Variant::num() const
{
    check_access(Tag::Int);
    return num_;
}

double& Variant::real()
{
    check_access(Tag::Double);
    return real_;
}

const double& Variant::real() const
{
    check_access(Tag::Double);
    return real_;
}

Variant::Tag Variant::tag() const
{
    return tag_;
}

void Variant::check_access(Variant::Tag tag) const
{
    if (tag_ != tag)
        throw std::runtime_error("Bad access");
}

Variant& Variant::operator=(const Variant& other)
{
    using std::string;
    if (this != &other) {

        if (this != &other) {
            if (tag_ == Tag::String && other.tag_ == Tag::String) {
                str_ = other.str_;
            }

            if (tag_ == Tag::String) str_.~string();

            switch (other.tag_) {
            case Tag::Int:
                num_ = other.num_;
                break;
            case Tag::Double:
                real_ = other.real_;
                break;
            case Tag::String:
                new(&str_) string(other.str_);
                break;
            }
            tag_ = other.tag_;
        }
    }
    return *this;
}

std::ostream& operator<<(std::ostream& os, const Variant& variant)
{
    switch (variant.tag()) {
    case Variant::Tag::String:
        return os << variant.str();
    case Variant::Tag::Int:
        return os << variant.num();
    case Variant::Tag::Double:
        return os << variant.real();
    }
}

