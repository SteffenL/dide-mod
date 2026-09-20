#pragma once

#include <string>
#include <string_view>

std::wstring widen_string(std::string_view input);
std::string narrow_string(std::wstring_view input);
