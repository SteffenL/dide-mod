#include "minhook.hpp"
#include "misc.hpp"

#include <format>

#include <MinHook.h>

namespace minhook {

namespace detail {
void create_hook_internal(const std::string& name, void* target, void* detour, void*& original) {
    const auto err{MH_CreateHook(reinterpret_cast<LPVOID>(target), reinterpret_cast<LPVOID>(detour),
                                 reinterpret_cast<LPVOID*>(&original))};
    if (err != MH_OK) {
        throw Error{std::format("Failed to create hook for {} ({})", name,
                                static_cast<std::underlying_type_t<decltype(err)>>(err))};
    }
}

void remove_hook_internal(const std::string& name, void* target) {
    if (const auto err{MH_RemoveHook(reinterpret_cast<LPVOID>(target))}; err != MH_OK) {
        throw Error{std::format("Failed to remove hook for {} ({})", name,
                                static_cast<std::underlying_type_t<decltype(err)>>(err))};
    }
}

void queue_enable_hook_internal(const std::string& name, void* target) {
    if (const auto err{MH_QueueEnableHook(reinterpret_cast<LPVOID>(target))}; err != MH_OK) {
        throw Error{std::format("Failed to queue hook enablement for {} ({})", name,
                                static_cast<std::underlying_type_t<decltype(err)>>(err))};
    }
}

void queue_disable_hook_internal(const std::string& name, void* target) {
    if (const auto err{MH_QueueDisableHook(reinterpret_cast<LPVOID>(target))}; err != MH_OK) {
        throw Error{std::format("Failed to queue hook disablement for {} ({})", name,
                                static_cast<std::underlying_type_t<decltype(err)>>(err))};
    }
}
} // namespace detail

void initialize() {
    if (const auto err{MH_Initialize()}; err != MH_OK) {
        throw Error{std::format("Failed to initialize MinHook ({})",
                                static_cast<std::underlying_type_t<decltype(err)>>(err))};
    }
}

void uninitialize() { MH_Uninitialize(); }

void apply_queued() {
    if (const auto err{MH_ApplyQueued()}; err != MH_OK) {
        throw Error{"Failed to apply queued hook changes"};
    }
}

} // namespace minhook
