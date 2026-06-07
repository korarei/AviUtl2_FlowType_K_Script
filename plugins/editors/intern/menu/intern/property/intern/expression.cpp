#include "expression.hpp"

#include <algorithm>
#include <array>
#include <format>
#include <string>
#include <string_view>

#include <intern/string.hpp>

#include "utilities.hpp"

namespace {
namespace string = flow::string;

constinit LOG_HANDLE *logger = nullptr;
constinit EDIT_HANDLE *editor = nullptr;

enum class ReferenceScope {
    Local,
    Object,
    Global,
};

constexpr void
copy_prop_name(ReferenceScope target, OBJECT_HANDLE handle, const wchar_t *fx, const wchar_t *prop) {
    std::wstring name;

    switch (target) {
        case ReferenceScope::Local:
            name = prop;
            break;
        case ReferenceScope::Object:
            name = std::format(L"{}.{}", fx, prop);
            break;
        case ReferenceScope::Global: {
            struct Context {
                OBJECT_HANDLE handle;
                int layer;
            } ctx{handle, 0};

            editor->call_edit_section_param(&ctx, [](void *param, EDIT_SECTION *edit) {
                auto *ctx = static_cast<Context *>(param);

                ctx->layer = edit->get_object_layer_frame(ctx->handle).layer + 1;
            });

            name = std::format(L"layer{}.{}.{}", ctx.layer, fx, prop);
            break;
        }
    }

    if (flow::editor::menu::property::set_clipboard_text(name))
        logger->info(logger, std::format(L"Copied '{}' to the clipboard", name).c_str());
    else
        logger->error(logger, std::format(L"Failed to copy '{}' to the clipboard", name).c_str());
}

constexpr void
copy_expression(OBJECT_HANDLE handle, const wchar_t *fx, const wchar_t *prop) {
    constexpr std::array subjects{
            EDIT_HANDLE::EFFECT_ITEM_TYPE_INTEGER,
            EDIT_HANDLE::EFFECT_ITEM_TYPE_NUMBER,
    };

    struct Context {
        OBJECT_HANDLE handle;
        const wchar_t *fx;
        const wchar_t *prop;
        int type;
    } ctx{handle, fx, prop, 0};

    if (!editor->enum_effect_item(
                flow::editor::menu::property::remove_suffix(fx).c_str(),
                &ctx,
                [](void *param, const wchar_t *prop, int type) {
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

        const auto sep = props.find('|');
        if (sep == std::string_view::npos) {
            logger->warn(logger, L"Expression not found");
            return;
        }

        const auto data = props.substr(0, sep);
        const auto cfg = props.substr(sep);

        if (std::ranges::count(data, ',') < 2)
            return;

        int flag;
        if (string::to_number(data.substr(data.rfind(',') + 1), flag) && (flag & 8) == 0) {
            logger->warn(logger, L"Expression not found");
            return;
        }

        const auto result = string::to_wstring(string::as_utf8(cfg.substr(1uz, cfg.find('|', 1uz) - 1uz)));

        if (flow::editor::menu::property::set_clipboard_text(result))
            logger->info(logger, std::format(L"Copied '{}' to the clipboard", result).c_str());
        else
            logger->error(logger, std::format(L"Failed to copy '{}' to the clipboard", result).c_str());
    });
}
}  // namespace

namespace flow::editor::menu::property::expression {
void
init(HOST_APP_TABLE *host, LOG_HANDLE *log_handle, EDIT_HANDLE *edit_handle) {
    logger = log_handle;
    ::editor = edit_handle;

    host->register_object_item_menu_param(
            L"FlowType_K\\プロパティ名をコピー\\{プロパティ名}",
            false,
            nullptr,
            []([[maybe_unused]] void *param, OBJECT_HANDLE handle, const wchar_t *fx, const wchar_t *prop) {
                copy_prop_name(ReferenceScope::Local, handle, fx, prop);
            });

    host->register_object_item_menu_param(
            L"FlowType_K\\プロパティ名をコピー\\{エフェクト名}.{プロパティ名}",
            false,
            nullptr,
            []([[maybe_unused]] void *param, OBJECT_HANDLE handle, const wchar_t *fx, const wchar_t *prop) {
                copy_prop_name(ReferenceScope::Object, handle, fx, prop);
            });

    host->register_object_item_menu_param(
            L"FlowType_K\\プロパティ名をコピー\\{レイヤー名}.{エフェクト名}.{プロパティ名}",
            false,
            nullptr,
            []([[maybe_unused]] void *param, OBJECT_HANDLE handle, const wchar_t *fx, const wchar_t *prop) {
                copy_prop_name(ReferenceScope::Global, handle, fx, prop);
            });

    host->register_object_item_menu_param(
            L"FlowType_K\\参照式をコピー",
            false,
            nullptr,
            []([[maybe_unused]] void *param, OBJECT_HANDLE handle, const wchar_t *fx, const wchar_t *prop) {
                copy_expression(handle, fx, prop);
            });
}
}  // namespace flow::editor::menu::property::expression
