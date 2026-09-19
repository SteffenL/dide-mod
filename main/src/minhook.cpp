#include "minhook.hpp"
#include "misc.hpp"

#include <MinHook.h>

namespace minhook {

namespace detail {
void create_hook_internal(const std::string& name, void* target, void* detour, void*& original) {
    const auto err{MH_CreateHook(reinterpret_cast<LPVOID>(target), reinterpret_cast<LPVOID>(detour),
                                 reinterpret_cast<LPVOID*>(&original))};
    if (err != MH_OK) {
        throw Error::format("Failed to create hook for {}", name);
    }
}

void remove_hook_internal(const std::string& name, void* target) {
    if (const auto err{MH_RemoveHook(reinterpret_cast<LPVOID>(target))}; err != MH_OK) {
        throw Error::format("Failed to remove hook for {}", name);
    }
}

void queue_enable_hook_internal(const std::string& name, void* target) {
    if (const auto err{MH_QueueEnableHook(reinterpret_cast<LPVOID>(target))}; err != MH_OK) {
        throw Error::format("Failed to queue hook enablement for {}", name);
    }
}

void queue_disable_hook_internal(const std::string& name, void* target) {
    if (const auto err{MH_QueueDisableHook(reinterpret_cast<LPVOID>(target))}; err != MH_OK) {
        throw Error::format("Failed to queue hook disablement for {}", name);
    }
}
} // namespace detail

void initialize() {
    if (MH_Initialize() != MH_OK) {
        throw Error{"Failed to initialize MinHook"};
    }
}

void uninitialize() { MH_Uninitialize(); }

void apply_queued() {
    if (const auto err{MH_ApplyQueued()}; err != MH_OK) {
        throw Error{"Failed to apply queued hook changes"};
    }
}

} // namespace minhook
