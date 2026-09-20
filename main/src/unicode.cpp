#include "unicode.hpp"

#include <limits>
#include <stdexcept>
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
