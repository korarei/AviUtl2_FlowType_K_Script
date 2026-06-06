#include "expression.hpp"

#include <format>
#include <string>

#include "utilities.hpp"

namespace {
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
}
}  // namespace flow::editor::menu::property::expression
