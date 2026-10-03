#include "factory.hpp"
#include "../config.hpp"
#include "../host.hpp"
#include "../misc.hpp"
#include "base.hpp"
#include "ce5/ce5_mod.hpp"
#include "ce6/ce6_mod.hpp"

#include <format>
#include <memory>
#include <string_view>

std::unique_ptr<ModBase> create_mod_for_host(const HostAppInfo& host_info, config::Config& config) {
    if (host_info.id == ce6::mod::dide_id || host_info.id == ce6::mod::dirde_id || host_info.id == ce6::mod::dl_id) {
        return std::make_unique<ce6::mod::Ce6Mod>(host_info, config);
    } else if (host_info.id == ce5::mod::di_id || host_info.id == ce5::mod::dir_id) {
        return std::make_unique<ce5::mod::Ce5Mod>(host_info, config);
    } else {
        throw Error{std::format("Unknown host ID: {}", host_info.id)};
    }
}
