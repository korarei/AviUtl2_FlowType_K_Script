#pragma once

#include <cstring>
#include <string>
#include <string_view>

#include <windows.h>

namespace flow::editor::menu::property {
[[nodiscard]] inline std::wstring
remove_suffix(std::wstring_view s) {
    const auto pos = s.rfind(L':');

    if (pos == std::wstring_view::npos)
        return std::wstring(s);

    return std::wstring(s.substr(0uz, pos));
}

[[nodiscard]] inline bool
set_clipboard_text(const std::wstring &text) {
    if (!OpenClipboard(nullptr))
        return false;

    if (!EmptyClipboard()) {
        CloseClipboard();
        return false;
    }

    const size_t bytes = (text.size() + 1) * sizeof(wchar_t);
    HGLOBAL hg = GlobalAlloc(GMEM_MOVEABLE, bytes);
    if (hg == nullptr) {
        CloseClipboard();
        return false;
    }

    void *ptr = GlobalLock(hg);
    if (ptr == nullptr) {
        GlobalFree(hg);
        CloseClipboard();
        return false;
    }

    std::memcpy(ptr, text.c_str(), bytes);

    GlobalUnlock(hg);

    if (SetClipboardData(CF_UNICODETEXT, hg) == nullptr) {
        GlobalFree(hg);
        CloseClipboard();
        return false;
    }

    CloseClipboard();

    return true;
}
}  // namespace flow::editor::menu::property
