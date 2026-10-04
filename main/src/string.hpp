#pragma once

#include <array>
#include <string>
#include <string_view>

std::wstring widen_string(std::string_view input);
std::string narrow_string(std::wstring_view input);

template<typename T>
constexpr std::array<T, 6> whitespace_chars = {
    T{'\t'}, T{'\n'}, T{'\v'}, T{'\f'}, T{'\r'}, T{' '},
};

template<typename T>
using StringViewOf = std::basic_string_view<typename T::value_type, typename T::traits_type>;

template<typename T>
constexpr StringViewOf<T> trim_left(const T& s) {
    using Char = typename T::value_type;
    StringViewOf<T> s_view{s};
    StringViewOf<T> ws_view{whitespace_chars<Char>.data(), whitespace_chars<Char>.size()};
    if (const auto non_ws_pos{s_view.find_first_not_of(ws_view)}; non_ws_pos != StringViewOf<T>::npos) {
        return s_view.substr(non_ws_pos);
    }
    return s;
}

template<typename T>
constexpr StringViewOf<T> trim_right(const T& s) {
    using Char = typename T::value_type;
    StringViewOf<T> s_view{s};
    StringViewOf<T> ws_view{whitespace_chars<Char>.data(), whitespace_chars<Char>.size()};
    if (const auto non_ws_pos{s_view.find_last_not_of(ws_view)}; non_ws_pos != StringViewOf<T>::npos) {
        return s_view.substr(0, non_ws_pos + 1);
    }
    return s;
}

template<typename T>
constexpr StringViewOf<T> trim(const T& s) {
    return trim_left(trim_right(s));
}
