#pragma once

#include "misc.hpp"

#include <concepts>
#include <cstdint>
#include <optional>
#include <string_view>

inline std::uint8_t decode_nibble(char c) {
    if (c >= '0' && c <= '9') {
        return c - '0';
    } else if (c >= 'A' && c <= 'F') {
        return c - 'A' + 0xa;
    } else if (c >= 'a' && c <= 'f') {
        return c - 'a' + 0xa;
    } else {
        throw Error{"Invalid input"};
    }
}

template<typename T>
    requires requires(T t) {
        typename T::element_type;
        typename T::size_type;
        { *t.begin() };
        { t.size() };
        requires std::integral<typename T::element_type>;
        sizeof(typename T::element_type) == 1;
        requires !std::same_as<typename T::element_type, bool>;
    }
std::optional<typename T::size_type> find_pattern(std::string_view pattern, const T& input) {
    if (pattern.size() % 2 != 0) {
        throw Error{"Pattern length is not aligned to 2 characters"};
    }
    if (pattern.size() / 2 > input.size()) {
        return std::nullopt;
    }
    for (typename T::size_type i{}; i < input.size(); ++i) {
        typename std::string_view::size_type match_count{};
        for (typename std::string_view::size_type j{}; j < pattern.size(); ++j) {
            if (pattern[j] != '?') {
                const auto bitshift{(j % 2) == 0 ? 4 : 0};
                if (((input[i + (j / 2)] >> bitshift) & 0xf) != decode_nibble(pattern[j])) {
                    break;
                }
            }
            ++match_count;
        }
        if (match_count >= pattern.size()) {
            return std::make_optional(i);
        }
    }
    return std::nullopt;
}
