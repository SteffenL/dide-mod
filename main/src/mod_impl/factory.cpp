#include "factory.hpp"
#include "../config.hpp"
#include "../host.hpp"
#include "../misc.hpp"
#include "ce5/ce5_mod.hpp"
#include "ce6/ce6_mod.hpp"

#include <format>
#include <string_view>

void run_mod_for_host(HostAppInfo host_info, config::Config config) {
    if (host_info.id == ce6::mod::dide_id || host_info.id == ce6::mod::dirde_id || host_info.id == ce6::mod::dl_id) {
        ce6::mod::ce6_mod_run(std::move(host_info), std::move(config));
    } else if (host_info.id == ce5::mod::di_id || host_info.id == ce5::mod::dir_id) {
        ce5::mod::ce5_mod_run(std::move(host_info), std::move(config));
    } else {
        throw Error{std::format("Unknown host ID: {}", host_info.id)};
    }
}
