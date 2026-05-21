#pragma once

#include <charconv>
#include <string>
#include <string_view>

#ifdef _WIN32
#include <windows.h>
#else
#include <filesystem>
#endif

namespace flow::string {
constexpr inline std::wstring
to_wstring(const std::u8string_view &string) {
#ifdef _WIN32
    if (string.empty())
        return {};

    const char *str = reinterpret_cast<const char *>(string.data());
    const int size = static_cast<int>(string.size());

    const int wsize = MultiByteToWideChar(CP_UTF8, 0, str, size, NULL, 0);
    std::wstring wstr(wsize, 0);
    MultiByteToWideChar(CP_UTF8, 0, str, size, wstr.data(), wsize);

    return wstr;
#else
    return std::filesystem::path(string).wstring();
#endif
}

constexpr inline std::u8string
to_utf8(const std::wstring_view &string) {
#ifdef _WIN32
    if (string.empty())
        return {};

    const int wsize = static_cast<int>(string.size());

    const int size = WideCharToMultiByte(CP_UTF8, 0, string.data(), wsize, NULL, 0, NULL, NULL);
    std::u8string utf8(size, 0);
    WideCharToMultiByte(CP_UTF8, 0, string.data(), wsize, reinterpret_cast<char *>(utf8.data()), size, NULL, NULL);

    return utf8;
#else
    return std::filesystem::path(string).u8string();
#endif
}

constexpr inline std::string
as_string(const std::u8string_view &string) {
    return std::string(reinterpret_cast<const char *>(string.data()), string.size());
}

constexpr inline std::string
as_string(const char *string) {
    return string != nullptr ? std::string(string) : std::string();
}

constexpr inline std::string_view
as_string_view(const char *string) {
    return string != nullptr ? std::string_view(string) : std::string_view();
}

constexpr inline std::u8string
as_utf8(const std::string_view &string) {
    return std::u8string(reinterpret_cast<const char8_t *>(string.data()), string.size());
}

[[nodiscard]] inline constexpr bool
to_number(std::string_view s, double &v) noexcept {
    const char *first = s.data();
    const char *last = first + s.size();
    const auto [ptr, ec] = std::from_chars(first, last, v);
    return ptr == last && ec == std::errc{};
}

[[nodiscard]] inline constexpr bool
to_number(std::string_view s, int &v) noexcept {
    const char *first = s.data();
    const char *last = first + s.size();
    const auto [ptr, ec] = std::from_chars(first, last, v);
    return ptr == last && ec == std::errc{};
}
}  // namespace flow::string
