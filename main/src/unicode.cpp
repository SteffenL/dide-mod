#include "unicode.hpp"

#include <array>
#include <limits>
#include <stdexcept>
#include <string>
#include <string_view>
#include <system_error>

#ifndef NOMINMAX
    #define NOMINMAX
#endif

#include <windows.h>

std::wstring widen_string(std::string_view input) {
    if (input.empty()) {
        return {};
    }
    if (input.size() > static_cast<std::size_t>(std::numeric_limits<int>::max())) {
        throw std::length_error{"Input is too long"};
    }
    const auto input_length{static_cast<int>(input.size())};
    const auto cp{CP_UTF8};
    const auto flags{MB_ERR_INVALID_CHARS};
    const auto* data{input.data()};
    auto required_length{::MultiByteToWideChar(cp, flags, data, input_length, nullptr, 0)};
    if (required_length > 0) {
        std::wstring output(static_cast<std::size_t>(required_length), L'\0');
        if (::MultiByteToWideChar(cp, flags, data, input_length, &output[0], required_length) > 0) {
            return output;
        }
    }
    const auto ec{static_cast<int>(::GetLastError())};
    throw std::system_error{ec, std::system_category(), "MultiByteToWideChar failed"};
}

std::string narrow_string(std::wstring_view input) {
    if (input.empty()) {
        return {};
    }
    if (input.size() > static_cast<std::size_t>(std::numeric_limits<int>::max())) {
        throw std::length_error{"Input is too long"};
    }
    const auto input_length{static_cast<int>(input.size())};
    const auto cp{CP_UTF8};
    const auto flags{WC_ERR_INVALID_CHARS};
    const auto* data{input.data()};
    const auto required_length{::WideCharToMultiByte(cp, flags, data, input_length, nullptr, 0, nullptr, nullptr)};
    if (required_length > 0) {
        std::string output(static_cast<std::size_t>(required_length), '\0');
        if (::WideCharToMultiByte(cp, flags, data, input_length, reinterpret_cast<char*>(&output[0]), required_length,
                                  nullptr, nullptr) > 0) {
            return output;
        }
    }
    const auto ec{static_cast<int>(::GetLastError())};
    throw std::system_error{ec, std::system_category(), "WideCharToMultiByte failed"};
}

template<typename T>
std::basic_string_view<T> trim_right(std::basic_string_view<T> s) {
    static constexpr std::array<T, 6> ws = {
        T{'\t'}, T{'\n'}, T{'\v'}, T{'\f'}, T{'\r'}, T{' '},
    };
    if (const auto non_ws_pos{s.find_last_not_of(std::basic_string_view<T>{ws.data(), ws.size()})};
        non_ws_pos != std::basic_string_view<T>::npos) {
        return s.substr(0, non_ws_pos);
    }
    return s;
}

template std::basic_string_view<char> trim_right(std::basic_string_view<char> s);
template std::basic_string_view<wchar_t> trim_right(std::basic_string_view<wchar_t> s);
