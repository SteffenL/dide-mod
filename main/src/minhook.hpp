#pragma once

#include <string>

namespace minhook {
namespace detail {
void create_hook_internal(const std::string& name, void* target, void* detour, void*& original);
void remove_hook_internal(const std::string& name, void* target);
void queue_enable_hook_internal(const std::string& name, void* target);
void queue_disable_hook_internal(const std::string& name, void* target);
} // namespace detail

void initialize();
void uninitialize();

template<typename Target, typename Detour, typename Original>
void create_hook(const std::string& name, Target target, Detour detour, Original& original) {
    return detail::create_hook_internal(name, reinterpret_cast<void*>(target), reinterpret_cast<void*>(detour),
                                        reinterpret_cast<void*&>(original));
}

template<typename Target>
void remove_hook(const std::string& name, Target target) {
    return detail::remove_hook_internal(name, reinterpret_cast<void*>(target));
}

template<typename Target>
void queue_enable_hook(const std::string& name, Target target) {
    return detail::queue_enable_hook_internal(name, reinterpret_cast<void*>(target));
}

template<typename Target>
void queue_disable_hook(const std::string& name, Target target) {
    detail::queue_disable_hook_internal(name, reinterpret_cast<void*>(target));
}

void apply_queued();

} // namespace minhook
