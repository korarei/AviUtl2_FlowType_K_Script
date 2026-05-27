#include "../property.hpp"

#include <algorithm>
#include <format>
#include <ranges>
#include <sstream>
#include <string>
#include <string_view>

#include <intern/alias/alias.hpp>
#include <intern/utilities.hpp>
#include <unordered_map>

namespace {
namespace string = flow::string;

using Object = flow::alias::Object;

constinit LOG_HANDLE *logger = nullptr;
constinit EDIT_HANDLE *editor = nullptr;

enum class CopyScope {
    All,
    Forward,
    Backward,
};

enum class InvertScope {
    Current,
    All,
    Forward,
    Backward,
};

enum class CopyFormat {
    Ini,
    Toml,
};

[[nodiscard]] inline constexpr std::wstring
remove_suffix(std::wstring_view s) {
    const auto pos = s.rfind(L':');

    if (pos == std::wstring_view::npos)
        return std::wstring(s);

    return std::wstring(s.substr(0uz, pos));
}

[[nodiscard]] constexpr bool
set_clipboard_text(const std::wstring &text) {
    if (!OpenClipboard(nullptr))
        return false;

    if (!EmptyClipboard())
        return false;

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

void
copy_value(CopyScope scope, OBJECT_HANDLE handle, const wchar_t *fx, const wchar_t *prop) {
    constexpr std::array subjects{
            EDIT_HANDLE::EFFECT_ITEM_TYPE_INTEGER,
            EDIT_HANDLE::EFFECT_ITEM_TYPE_NUMBER,
            EDIT_HANDLE::EFFECT_ITEM_TYPE_CHECK,
    };

    struct Context {
        CopyScope scope;
        OBJECT_HANDLE handle;
        const wchar_t *fx;
        const wchar_t *prop;
        int type;
    } ctx{scope, handle, fx, prop, 0};

    if (!editor->enum_effect_item(remove_suffix(fx).c_str(), &ctx, [](void *param, const wchar_t *prop, int type) {
            auto *ctx = static_cast<Context *>(param);

            if (std::wstring_view(ctx->prop) == prop)
                ctx->type = type;
        }))
        return;

    if (!std::ranges::contains(subjects, ctx.type))
        return;

    editor->call_edit_section_param(&ctx, [](void *param, EDIT_SECTION *edit) {
        const auto *ctx = static_cast<const Context *>(param);

        const auto props = string::as_string_view(edit->get_object_item_value(ctx->handle, ctx->fx, ctx->prop));

        if (props.empty()) {
            logger->error(logger, L"Failed to get the value");
            return;
        }

        const auto index = edit->get_focus_object_section();

        if (index < 0) {
            logger->error(logger, L"Failed to get the focus section");
            return;
        }

        int st = 0, ed = 0;

        switch (ctx->scope) {
            case CopyScope::All:
                ed = edit->get_object_section_num(ctx->handle);
                break;
            case CopyScope::Forward:
                st = index;
                ed = edit->get_object_section_num(ctx->handle);
                break;
            case CopyScope::Backward:
                ed = index;
                break;
        }

        auto values = props | std::views::split(',');
        auto remaining = values | std::views::drop(1);

        if (remaining.begin() == remaining.end()) {
            return;
        } else if (ctx->type != EDIT_HANDLE::EFFECT_ITEM_TYPE_CHECK) {
            // 中間点無視
            double dummy;
            if (!string::to_number(std::string_view(*(values | std::views::drop(2)).begin()), dummy)) {
                switch (ctx->scope) {
                    case CopyScope::All:
                    case CopyScope::Forward:
                        st = 0, ed = 1;
                        break;
                    case CopyScope::Backward:
                        return;
                }
            }
        }

        auto targets = values | std::views::take(ed + 1) | std::views::drop(st);
        if (targets.begin() == targets.end())
            return;

        const std::string_view src(*(values | std::views::drop(index)).begin());

        const auto result = std::ranges::to<std::string>(
                values | std::views::enumerate | std::views::transform([=](auto &&pair) -> std::string_view {
                    const auto [i, v] = pair;
                    // トラックバー: [0, n]
                    // チェックボックス: [0, n - 1] (これ以降の要素がないのでトラックバーと同一で可)
                    return i >= st && i <= ed ? src : std::string_view(v);
                })
                | std::views::join_with(','));

        edit->set_object_item_value(ctx->handle, ctx->fx, ctx->prop, result.c_str());

        logger->info(
                logger,
                std::format(L"Updated '{}:{}' to '{}'", ctx->fx, ctx->prop, string::to_wstring(string::as_utf8(result)))
                        .c_str());
    });
}

constexpr void
invert_values(InvertScope scope, OBJECT_HANDLE handle, const wchar_t *fx, const wchar_t *prop) {
    constexpr std::array subjects{
            EDIT_HANDLE::EFFECT_ITEM_TYPE_INTEGER,
            EDIT_HANDLE::EFFECT_ITEM_TYPE_NUMBER,
            EDIT_HANDLE::EFFECT_ITEM_TYPE_CHECK,
    };

    struct Context {
        InvertScope scope;
        OBJECT_HANDLE handle;
        const wchar_t *fx;
        const wchar_t *prop;
        int type;
    } ctx{scope, handle, fx, prop, 0};

    if (!editor->enum_effect_item(remove_suffix(fx).c_str(), &ctx, [](void *param, const wchar_t *prop, int type) {
            auto *ctx = static_cast<Context *>(param);

            if (std::wstring_view(ctx->prop) == prop)
                ctx->type = type;
        }))
        return;

    if (!std::ranges::contains(subjects, ctx.type))
        return;

    editor->call_edit_section_param(&ctx, [](void *param, EDIT_SECTION *edit) {
        const auto *ctx = static_cast<const Context *>(param);

        const auto props = string::as_string_view(edit->get_object_item_value(ctx->handle, ctx->fx, ctx->prop));

        if (props.empty()) {
            logger->error(logger, L"Failed to get the value");
            return;
        }

        const auto index = edit->get_focus_object_section();

        if (index < 0) {
            logger->error(logger, L"Failed to get the focus section");
            return;
        }

        int st = 0, ed = 0;

        switch (ctx->scope) {
            case InvertScope::Current:
                st = ed = index;
                break;
            case InvertScope::All:
                ed = edit->get_object_section_num(ctx->handle);
                break;
            case InvertScope::Forward:
                st = index;
                ed = edit->get_object_section_num(ctx->handle);
                break;
            case InvertScope::Backward:
                ed = index;
                break;
        }

        auto values = props | std::views::split(',');
        auto remaining = values | std::views::drop(1);

        if (remaining.begin() == remaining.end()) {
            st = ed = 0;
        } else if (ctx->type != EDIT_HANDLE::EFFECT_ITEM_TYPE_CHECK) {
            // 中間点無視
            double dummy;
            if (!string::to_number(std::string_view(*(values | std::views::drop(2)).begin()), dummy)) {
                switch (ctx->scope) {
                    case InvertScope::All:
                    case InvertScope::Forward:
                        st = 0, ed = 1;
                        break;
                    case InvertScope::Current:
                    case InvertScope::Backward:
                        st = ed = 0;
                        break;
                }
            }
        }

        auto targets = values | std::views::take(ed + 1) | std::views::drop(st);
        if (targets.begin() == targets.end())
            return;

        std::string result;

        switch (ctx->type) {
            case EDIT_HANDLE::EFFECT_ITEM_TYPE_INTEGER:
            case EDIT_HANDLE::EFFECT_ITEM_TYPE_NUMBER:
                result = std::ranges::to<std::string>(
                        values | std::views::enumerate | std::views::transform([=](auto &&pair) -> std::string {
                            const auto [i, v] = pair;
                            const std::string_view sv(v);

                            if (!sv.empty() && i >= st && i <= ed)
                                return sv.starts_with("-") ? std::string(sv.substr(1uz)) : "-" + std::string(sv);

                            return std::string(sv);
                        })
                        | std::views::join_with(','));
                break;
            case EDIT_HANDLE::EFFECT_ITEM_TYPE_CHECK:
                result = std::ranges::to<std::string>(
                        values | std::views::enumerate | std::views::transform([=](auto &&pair) -> std::string_view {
                            const auto [i, v] = pair;
                            const std::string_view sv(v);

                            if (!sv.empty() && i >= st && i <= ed)
                                return sv == "0" ? "1" : "0";

                            return sv;
                        })
                        | std::views::join_with(','));
                break;
        }

        edit->set_object_item_value(ctx->handle, ctx->fx, ctx->prop, result.c_str());

        logger->info(
                logger,
                std::format(L"Updated '{}:{}' to '{}'", ctx->fx, ctx->prop, string::to_wstring(string::as_utf8(result)))
                        .c_str());
    });
}

constexpr void
copy_fx(CopyFormat format, EDIT_SECTION *edit, OBJECT_HANDLE handle, const wchar_t *fx) {
    const auto alias = edit->get_object_alias(handle);
    if (alias == nullptr) {
        logger->error(logger, L"Failed to get the alias of the object");
        return;
    }

    const Object object(alias);

    const std::string key = string::as_string(string::to_utf8(fx));
    const size_t sep = key.find(":");

    std::string name;
    int suffix = 0;

    if (sep != std::string_view::npos && string::to_number(key.substr(sep + 1uz), suffix))
        name = key.substr(0uz, sep);
    else
        name = key;

    int index = 0, count = 0;
    auto effect = object.get(index);

    while (true) {
        if (effect.get("effect.name") == name && suffix == count++)
            break;

        effect = object.get(++index);
    }

    std::string result;

    switch (format) {
        case CopyFormat::Ini:
            result = effect.alias();
            result.replace(0uz, std::format("[Object.{}]", index).length(), "[Effect.0]");
            break;
        case CopyFormat::Toml: {
            struct Context {
                OBJECT_HANDLE handle;
                const wchar_t *fx;
                std::unordered_map<std::string, int> type;
            } ctx{handle, fx, {}};

            editor->enum_effect_item(remove_suffix(fx).c_str(), &ctx, [](void *param, const wchar_t *prop, int type) {
                auto *ctx = static_cast<Context *>(param);
                ctx->type[string::as_string(string::to_utf8(prop))] = type;
            });

            std::string tmp(effect.alias());
            tmp.replace(0uz, std::format("[Object.{}]", index).length(), std::format("[\"{}\"]", name));

            result.reserve(tmp.size());

            std::istringstream iss(tmp);
            std::string line;

            while (std::getline(iss, line)) {
                std::string_view sv(line);

                if (!sv.empty() && sv.back() == '\r')
                    sv.remove_suffix(1);

                if (sv.empty()) {
                    result += "\n";
                    continue;
                }

                if (sv.starts_with("effect.name="))
                    continue;

                if (sv.starts_with('[') && sv.ends_with(']')) {
                    result += std::format("{}\n", sv);
                    continue;
                }

                const std::string_view pair = sv.substr(0uz, sv.find(','));

                const size_t eq = pair.find('=');
                if (eq == std::string_view::npos) {
                    result += std::format("{}\n", pair);
                    continue;
                }

                const std::string_view k = pair.substr(0uz, eq), v = pair.substr(eq + 1uz);

                const auto it = ctx.type.find(std::string(k));
                if (it != ctx.type.end()) {
                    switch (it->second) {
                        case EDIT_HANDLE::EFFECT_ITEM_TYPE_COLOR:
                            result += std::format("\"{}\"=0x{}\n", k, v);
                            break;
                        case EDIT_HANDLE::EFFECT_ITEM_TYPE_CHECK:
                            result += std::format("\"{}\"={}\n", k, v == "0" ? "false" : "true");
                            break;
                        case EDIT_HANDLE::EFFECT_ITEM_TYPE_INTEGER:
                            result += std::format("\"{}\"={}\n", k, v);
                            break;
                        case EDIT_HANDLE::EFFECT_ITEM_TYPE_NUMBER: {
                            const size_t ed = v.find_last_not_of('0');
                            if (ed != std::string_view::npos)
                                if (v[ed] == '.')
                                    result += std::format("\"{}\"={}\n", k, v.substr(0uz, ed + 2uz));
                                else
                                    result += std::format("\"{}\"={}\n", k, v.substr(0uz, ed + 1uz));
                            else
                                result += std::format("\"{}\"={}\n", k, v);

                            break;
                        }
                        default:
                            result += std::format("\"{}\"=\"{}\"\n", k, v);
                            break;
                    }
                } else {
                    result += std::format("\"{}\"=\"{}\"\n", k, v);
                }
            }

            break;
        }
    }

    if (set_clipboard_text(string::to_wstring(string::as_utf8(result))))
        logger->info(
                logger,
                std::format(
                        L"Copied '{}' to the clipboard in {} format", fx, format == CopyFormat::Toml ? L"TOML" : L"INI")
                        .c_str());
    else
        logger->error(
                logger,
                std::format(
                        L"Failed to copy '{}' to the clipboard in {} format",
                        fx,
                        format == CopyFormat::Toml ? L"TOML" : L"INI")
                        .c_str());
}
}  // namespace

namespace flow::editors::menu::property {
void
init(HOST_APP_TABLE *host, LOG_HANDLE *handle) {
    logger = handle;
    editor = host->create_edit_handle();

    host->register_object_item_menu_param(
            L"FlowType_K\\値を揃える\\全ての区間",
            false,
            nullptr,
            []([[maybe_unused]] void *param, OBJECT_HANDLE handle, const wchar_t *fx, const wchar_t *prop) {
                copy_value(CopyScope::All, handle, fx, prop);
            });

    host->register_object_item_menu_param(
            L"FlowType_K\\値を揃える\\以前の区間",
            false,
            nullptr,
            []([[maybe_unused]] void *param, OBJECT_HANDLE handle, const wchar_t *fx, const wchar_t *prop) {
                copy_value(CopyScope::Backward, handle, fx, prop);
            });

    host->register_object_item_menu_param(
            L"FlowType_K\\値を揃える\\以降の区間",
            false,
            nullptr,
            []([[maybe_unused]] void *param, OBJECT_HANDLE handle, const wchar_t *fx, const wchar_t *prop) {
                copy_value(CopyScope::Forward, handle, fx, prop);
            });

    host->register_object_item_menu_param(
            L"FlowType_K\\値を反転\\現在の区間",
            false,
            nullptr,
            []([[maybe_unused]] void *param, OBJECT_HANDLE handle, const wchar_t *fx, const wchar_t *prop) {
                invert_values(InvertScope::Current, handle, fx, prop);
            });

    host->register_object_item_menu_param(
            L"FlowType_K\\値を反転\\全ての区間",
            false,
            nullptr,
            []([[maybe_unused]] void *param, OBJECT_HANDLE handle, const wchar_t *fx, const wchar_t *prop) {
                invert_values(InvertScope::All, handle, fx, prop);
            });

    host->register_object_item_menu_param(
            L"FlowType_K\\値を反転\\以前の区間",
            false,
            nullptr,
            []([[maybe_unused]] void *param, OBJECT_HANDLE handle, const wchar_t *fx, const wchar_t *prop) {
                invert_values(InvertScope::Backward, handle, fx, prop);
            });

    host->register_object_item_menu_param(
            L"FlowType_K\\値を反転\\以降の区間",
            false,
            nullptr,
            []([[maybe_unused]] void *param, OBJECT_HANDLE handle, const wchar_t *fx, const wchar_t *prop) {
                invert_values(InvertScope::Forward, handle, fx, prop);
            });

    host->register_object_item_menu(
            L"FlowType_K\\エフェクトをコピー\\エイリアス形式 (INI 形式)",
            true,
            [](EDIT_SECTION *edit, OBJECT_HANDLE handle, const wchar_t *fx, [[maybe_unused]] const wchar_t *prop) {
                copy_fx(CopyFormat::Ini, edit, handle, fx);
            });

    host->register_object_item_menu(
            L"FlowType_K\\エフェクトをコピー\\FlowType_K 形式 (TOML 形式)",
            true,
            [](EDIT_SECTION *edit, OBJECT_HANDLE handle, const wchar_t *fx, [[maybe_unused]] const wchar_t *prop) {
                copy_fx(CopyFormat::Toml, edit, handle, fx);
            });
}
}  // namespace flow::editors::menu::property
