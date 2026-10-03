#pragma once

#include "../../config.hpp"
#include "../../host.hpp"

#include <string_view>

namespace ce6::mod {

constexpr std::string_view dide_id{"DeadIslandDE"};
constexpr std::string_view dirde_id{"DeadIslandRiptideDE"};
constexpr std::string_view dl_id{"DyingLight"};
constexpr std::string_view engine_dll_name{"engine_x64_rwdi.dll"};
constexpr std::string_view filesystem_dll_name{"filesystem_x64_rwdi.dll"};
constexpr std::string_view game_dll_name{"gamedll_x64_rwdi.dll"};

void ce6_mod_run(HostAppInfo host_info, config::Config config);

} // namespace ce6::mod
