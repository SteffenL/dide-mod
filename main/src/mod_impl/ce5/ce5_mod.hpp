#pragma once

#include "../../config.hpp"
#include "../../host.hpp"

#include <string_view>

namespace ce5::mod {

constexpr std::string_view di_id{"DeadIsland"};
constexpr std::string_view dir_id{"DeadIsland Riptide"};
constexpr std::string_view engine_dll_name{"engine_x86_rwdi.dll"};
constexpr std::string_view filesystem_dll_name{"filesystem_x86_rwdi.dll"};
constexpr std::string_view di_game_dll_name{"game_x86_rwdi.dll"};
constexpr std::string_view dir_game_dll_name{"gamedll_x86_rwdi.dll"};

void ce5_mod_run(HostAppInfo host_info, config::Config config);

} // namespace ce5::mod
