#pragma once

#include <functional>
#include <string_view>

void create_startup_hook(std::function<void(std::string_view)> callback);
