#ifndef FORM_H
#define FORM_H

#include<type_traits>
#include<ios>

#ifndef __cpp_lib_void_t
template<typename... Ts>
struct make_void {
    typedef void type;
};

template<typename... Ts> using void_t = typename make_void<Ts...>::type;
#endif

template<class T, class CharT, class = void>
struct is_output_streamable : std::false_type {
};

template<class T, class CharT>
struct is_output_streamable
        <T,
         CharT,
         std::void_t<decltype(std::declval<std::basic_ostream<CharT>&>() << std::declval<T>())>
        > : std::true_type {
};

template<class CharT>
class BasicForm;

template<typename T,
         typename CharT,
         typename = std::enable_if<is_output_streamable<T, CharT>::value>>
struct BoundForm {
public:
    const BasicForm<CharT>& form;
    const T& value;
};

template<class CharT>
class BasicForm {
public:
    void setf(std::ios_base::fmtflags flags) { fmtflags_ = flags | fmtflags_; }

    void setf(std::ios_base::fmtflags flags, std::ios_base::fmtflags mask)
    {
        fmtflags_ = (fmtflags_ & ~mask) | (flags & mask);
    }

    void unset(std::ios_base::fmtflags flags) { setf(0, flags); }

    std::streamsize precision() const { return precision_; }

    std::streamsize precision(std::streamsize new_precision)
    {
        std::streamsize old = precision_;
        precision_ = new_precision;
        return old;
    }

    std::streamsize width() const { return width_; }

    std::streamsize width(std::streamsize new_width)
    {
        std::streamsize old = width_;
        width_ = new_width;
        return old;
    }

    std::ios_base::fmtflags fmtflags() const { return fmtflags_; }

    std::ios_base::fmtflags fmtflags(std::ios_base::fmtflags new_flags)
    {
        std::ios_base::fmtflags old = fmtflags_;
        fmtflags_ = new_flags;
        return old;
    }

    CharT fill() const { return fill_; }

    CharT fill(CharT new_fill)
    {
        CharT old = fill_;
        fill_ = new_fill;
        return old;
    }

    explicit BasicForm(std::streamsize prec = 6, std::ios_base::fmtflags flags = {},
                       std::streamsize width = 0, char fill = ' ')
            : precision_(prec), fmtflags_(flags), width_(width), fill_(fill) { }

    template<typename T>
    BoundForm<T, CharT> operator()(const T& d) const
    {
        return BoundForm<T, CharT>{*this, d};
    }

private:
    std::streamsize precision_;
    std::streamsize width_;
    std::ios_base::fmtflags fmtflags_;
    CharT fill_;
};

template<typename T, typename CharT>
std::basic_ostream<CharT>& operator<<(std::basic_ostream<CharT>& os,
                                      const BoundForm<T, CharT>& bf)
{
    auto old_precision = os.precision(bf.form.precision());
    auto old_width = os.width(bf.form.width());
    auto old_fmtflags = os.flags(bf.form.fmtflags());
    auto old_fill = os.fill(bf.form.fill());

    os << bf.value;
    os.width(old_width);
    os.precision(old_precision);
    os.flags(old_fmtflags);
    os.fill(old_fill);
    return os;
}

using Form = BasicForm<char>;
using WideForm = BasicForm<wchar_t>;

#endif //FORM_H
