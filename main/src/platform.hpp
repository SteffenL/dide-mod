#pragma once

#include <filesystem>
#include <functional>
#include <optional>
#include <string>

uintptr_t get_base_of_code(uintptr_t image_base);
uintptr_t get_size_of_code(uintptr_t image_base);
std::filesystem::path exe_path();
std::filesystem::path exe_dir();
void msgbox_error(const std::string& text, const std::string& title);
[[noreturn]] void kill_current_process(int exit_code);
void set_post_process_init_routine(std::function<void()> callback);
bool is_loader_lock_held_by_current_thread();
std::optional<std::string> get_wine_version_str();
