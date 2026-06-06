#include "effect.hpp"

#include <format>

#include <string>
#include <string_view>
#include <unordered_map>

#include <intern/alias/alias.hpp>
#include <intern/string.hpp>

#include "utilities.hpp"

namespace {
namespace string = flow::string;

using Object = flow::alias::Object;

constinit LOG_HANDLE *logger = nullptr;
constinit EDIT_HANDLE *editor = nullptr;

enum class CopyFormat {
    Ini,
    Toml,
};

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

            editor->enum_effect_item(
                    flow::editor::menu::property::remove_suffix(fx).c_str(),
                    &ctx,
                    [](void *param, const wchar_t *prop, int type) {
                        auto *ctx = static_cast<Context *>(param);
                        ctx->type[string::as_string(string::to_utf8(prop))] = type;
                    });

            std::string tmp(effect.alias());
            tmp.replace(0uz, std::format("[Object.{}]", index).length(), std::format("[\"{}\"]", name));

            result.reserve(tmp.size());

            std::string_view remaining(tmp);

            while (!remaining.empty()) {
                const auto nl = remaining.find('\n');
                std::string_view line = remaining.substr(0, nl);
                remaining = nl != std::string_view::npos ? remaining.substr(nl + 1) : std::string_view{};

                if (!line.empty() && line.back() == '\r')
                    line.remove_suffix(1);

                if (line.empty()) {
                    result += "\n";
                    continue;
                }

                if (line.starts_with("effect.name="))
                    continue;

                if (line.starts_with('[') && line.ends_with(']')) {
                    result += std::format("{}\n", line);
                    continue;
                }

                const std::string_view pair = line.substr(0uz, line.find(','));

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
                            if (v.find('.') != std::string_view::npos) {
                                const size_t ed = v.find_last_not_of('0');
                                if (ed != std::string_view::npos)
                                    if (v[ed] == '.')
                                        result += std::format("\"{}\"={}\n", k, v.substr(0uz, ed + 2uz));
                                    else
                                        result += std::format("\"{}\"={}\n", k, v.substr(0uz, ed + 1uz));
                                else
                                    result += std::format("\"{}\"={}\n", k, v);
                            } else {
                                result += std::format("\"{}\"={}\n", k, v);
                            }

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

    if (flow::editor::menu::property::set_clipboard_text(string::to_wstring(string::as_utf8(result))))
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

namespace flow::editor::menu::property::effect {
void
init(HOST_APP_TABLE *host, LOG_HANDLE *log_handle, EDIT_HANDLE *edit_handle) {
    logger = log_handle;
    ::editor = edit_handle;

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
}  // namespace flow::editor::menu::property::effect
