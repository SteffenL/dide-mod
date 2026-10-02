#include <cassert>
#include <format>
#include <print>
#include <cstdint>
#include <functional>
#include <source_location>
#include <stdexcept>
#include <string>
#include <concepts>
#include <cstdint>
#include <optional>
#include <string_view>
#include <span>
#include <fstream>
#include <sstream>


class SourceLocation {
public:
    static consteval SourceLocation current(std::source_location loc = std::source_location::current()) noexcept {
        SourceLocation res;
        res.m_line = loc.line();
        res.m_column = loc.column();

        std::string_view function_name_sv{loc.function_name()};
        function_name_sv.copy(res.m_function_name, std::min<std::string_view::size_type>(
                                                       function_name_sv.size(), sizeof(res.m_function_name) - 1));

        std::string_view file_name_sv{loc.file_name()};
        if (!file_name_sv.empty()) {
            const auto slash_pos{file_name_sv.find_last_of("\\/")};
            if (slash_pos != std::string_view::npos) {
                const auto start_pos{slash_pos + 1};
                file_name_sv.copy(
                    res.m_file_name,
                    std::min<std::string_view::size_type>(file_name_sv.size() - start_pos, sizeof(res.m_file_name) - 1),
                    start_pos);
            }
        }

        return res;
    }

    std::uint_least32_t line() const noexcept { return m_line; }
    std::uint_least32_t column() const noexcept { return m_column; }
    const char* function_name() const noexcept { return m_function_name; };
    const char* file_name() const noexcept { return m_file_name; };

private:
    char m_function_name[128]{};
    char m_file_name[64]{};
    std::uint_least32_t m_line{};
    std::uint_least32_t m_column{};
};

class Error : public std::runtime_error {
public:
    Error(std::string message, SourceLocation loc = SourceLocation::current())
            : runtime_error{std::move(message)}, m_loc{std::move(loc)} {}

    const SourceLocation& where() const noexcept { return m_loc; }

private:
    SourceLocation m_loc;
};

template<typename T>
class NotNull {
public:
    using type = std::remove_pointer_t<T>;
    using pointer_type = std::add_pointer_t<type>;

    NotNull(pointer_type ptr) : m_ptr{ptr} {
        if (!m_ptr) {
            throw std::logic_error{"Pointer cannot be null"};
        }
    }

    template<typename U>
        requires(!std::same_as<U, T> && std::convertible_to<std::add_pointer_t<std::remove_pointer_t<U>>, pointer_type>)
    NotNull(const NotNull<U>& other) : m_ptr{other.get()} {}

    pointer_type get() const noexcept { return m_ptr; }
    pointer_type operator->() const noexcept { return m_ptr; }

    template<typename U = type>
    U& operator*() const noexcept
        requires(!std::is_void_v<type>)
    {
        return *m_ptr;
    }

    template<typename... Args>
    auto operator()(Args&&... args) {
        return std::invoke(m_ptr, std::forward<Args>(args)...);
    }

private:
    pointer_type m_ptr{};
};

template<typename Callable>
std::exception_ptr catch_exception(Callable callable) noexcept {
    try {
        callable();
        return nullptr;
    } catch (...) {
        return std::current_exception();
    }
}


inline std::uint8_t decode_nibble(char c) {
    if (c >= '0' && c <= '9') {
        return c - '0';
    } else if (c >= 'A' && c <= 'F') {
        return c - 'A' + 0xa;
    } else if (c >= 'a' && c <= 'f') {
        return c - 'a' + 0xa;
    } else {
        throw Error{"Invalid input"};
    }
}

template<typename T>
    requires requires(T t) {
        typename T::element_type;
        typename T::size_type;
        { *t.begin() };
        { t.size() };
        requires std::integral<typename T::element_type>;
        sizeof(typename T::element_type) == 1;
        requires !std::same_as<typename T::element_type, bool>;
    }
std::optional<typename T::size_type> find_pattern(std::string_view pattern, const T& input) {
    if (pattern.size() % 2 != 0) {
        throw Error{"Pattern length is not aligned to 2 characters"};
    }
    if (pattern.size() / 2 > input.size()) {
        return std::nullopt;
    }
    for (typename T::size_type i{}; i < input.size(); ++i) {
        typename std::string_view::size_type match_count{};
        for (typename std::string_view::size_type j{}; j < pattern.size(); ++j) {
            if (pattern[j] != '?') {
                const auto bitshift{(j % 2) == 0 ? 4 : 0};
                if (((input[i + (j / 2)] >> bitshift) & 0xf) != decode_nibble(pattern[j])) {
                    break;
                }
            }
            ++match_count;
        }
        if (match_count >= pattern.size()) {
            return std::make_optional(i);
        }
    }
    return std::nullopt;
}

int main(){
    std::ifstream file{"data.bin", std::ios_base::binary};
    std::stringstream datastream;
    datastream << file.rdbuf();
    std::string data{std::move(datastream).str()};
    const auto match_offset{find_pattern("5E5?C605??????1001B0015B8?????C20800", std::span{data})};
    std::println("got match_offset? {}", match_offset.has_value());
    assert(match_offset);
    std::println("at {:#x}", *match_offset);
    assert(*match_offset == 0xA2E0C);
    return 0;
}