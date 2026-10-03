#pragma once

#include "../config.hpp"
#include "../host.hpp"
#include "base.hpp"

#include <memory>

std::unique_ptr<ModBase> create_mod_for_host(const HostAppInfo& host_info, config::Config& config);
