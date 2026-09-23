#pragma once

#include <string>
#include <string_view>

std::wstring widen_string(std::string_view input);
std::string narrow_string(std::wstring_view input);

template<typename T>
std::basic_string_view<T> trim_right(std::basic_string_view<T> s);
