#include "value.hpp"

#include <algorithm>
#include <array>
#include <format>
#include <ranges>
#include <string>
#include <string_view>

#include <intern/string.hpp>

#include "utilities.hpp"

namespace {
namespace string = flow::string;

constinit LOG_HANDLE *logger = nullptr;
constinit EDIT_HANDLE *context = nullptr;

enum class TargetRange {
    Current,
    All,
    Preceding,
    Subsequent,
};

constexpr void
overwrite_values(TargetRange scope, OBJECT_HANDLE handle, const wchar_t *fx, const wchar_t *prop) {
    constexpr std::array subjects{
            EDIT_HANDLE::EFFECT_ITEM_TYPE_INTEGER,
            EDIT_HANDLE::EFFECT_ITEM_TYPE_NUMBER,
            EDIT_HANDLE::EFFECT_ITEM_TYPE_CHECK,
    };

    struct Context {
        TargetRange scope;
        OBJECT_HANDLE handle;
        const wchar_t *fx;
        const wchar_t *prop;
        int type;
    } ctx{scope, handle, fx, prop, 0};

    if (!context->enum_effect_item(
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

    context->call_edit_section_param(&ctx, [](void *param, EDIT_SECTION *edit) {
        const auto *ctx = static_cast<const Context *>(param);

        const auto props = string::as_string_view(edit->get_object_item_value(ctx->handle, ctx->fx, ctx->prop));

        if (props.empty()) {
            logger->error(logger, L"Failed to get the value");
            return;
        }

        auto index = edit->get_focus_object_section(), num = edit->get_object_section_num(ctx->handle);

        if (index < 0) {
            logger->error(logger, L"Failed to get the focus section");
            return;
        }

        const auto sep = props.find('|');
        const auto data = props.substr(0, sep);
        const auto cfg = sep != std::string_view::npos ? props.substr(sep) : std::string_view{};
        auto values = data | std::views::split(',');

        auto remaining = values | std::views::drop(1);
        if (remaining.begin() == remaining.end())
            return;

        if (ctx->type == EDIT_HANDLE::EFFECT_ITEM_TYPE_CHECK) {
            num -= 1;

            if (std::ranges::count(data, ',') != num)
                return;

            if (!cfg.empty()) {
                int flag;
                if (string::to_number(cfg.substr(1uz, cfg.find('|', 1uz) - 1uz), flag) && (flag & 1) == 0)
                    return;
            }
        } else {
            const int n = static_cast<int>(std::ranges::count(data, ','));

            if (n != 3 && n != num + 2)
                return;

            if (n == 3 && num > 1) {
                int flag;
                if (string::to_number(data.substr(data.rfind(',') + 1), flag) && (flag & 4) != 0)
                    index = 0, num = 1;
            }
        }

        int st = 0, ed = 0;

        switch (ctx->scope) {
            case TargetRange::All:
                ed = num;
                break;
            case TargetRange::Subsequent:
                st = index, ed = num;
                break;
            case TargetRange::Preceding:
                ed = index;
                break;
            default:
                break;
        }

        auto targets = values | std::views::take(ed + 1) | std::views::drop(st);
        if (targets.begin() == targets.end())
            return;

        const std::string_view src(*(values | std::views::drop(index)).begin());

        auto result = std::ranges::to<std::string>(
                values | std::views::enumerate | std::views::transform([=](auto &&pair) -> std::string_view {
                    const auto [i, v] = pair;
                    return i >= st && i <= ed ? src : std::string_view(v);
                })
                | std::views::join_with(','));

        if (sep != std::string_view::npos)
            result += cfg;

        if (edit->set_object_item_value(ctx->handle, ctx->fx, ctx->prop, result.c_str()))
            logger->info(logger, std::format(L"Updated '{}:{}'", ctx->fx, ctx->prop).c_str());
    });
}

constexpr void
invert_values(TargetRange scope, OBJECT_HANDLE handle, const wchar_t *fx, const wchar_t *prop) {
    constexpr std::array subjects{
            EDIT_HANDLE::EFFECT_ITEM_TYPE_INTEGER,
            EDIT_HANDLE::EFFECT_ITEM_TYPE_NUMBER,
            EDIT_HANDLE::EFFECT_ITEM_TYPE_CHECK,
    };

    struct Context {
        TargetRange scope;
        OBJECT_HANDLE handle;
        const wchar_t *fx;
        const wchar_t *prop;
        int type;
    } ctx{scope, handle, fx, prop, 0};

    if (!context->enum_effect_item(
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

    context->call_edit_section_param(&ctx, [](void *param, EDIT_SECTION *edit) {
        auto *ctx = static_cast<Context *>(param);

        const auto props = string::as_string_view(edit->get_object_item_value(ctx->handle, ctx->fx, ctx->prop));

        if (props.empty()) {
            logger->error(logger, L"Failed to get the value");
            return;
        }

        auto index = edit->get_focus_object_section(), num = edit->get_object_section_num(ctx->handle);

        if (index < 0) {
            logger->error(logger, L"Failed to get the focus section");
            return;
        }

        const auto sep = props.find('|');
        const auto data = props.substr(0, sep);
        const auto cfg = sep != std::string_view::npos ? props.substr(sep) : std::string_view{};
        auto values = data | std::views::split(',');

        auto remaining = values | std::views::drop(1);
        if (remaining.begin() == remaining.end())
            index = num = 0;

        if (ctx->type == EDIT_HANDLE::EFFECT_ITEM_TYPE_CHECK) {
            num -= 1;

            if (std::ranges::count(data, ',') != num)
                return;

            if (!cfg.empty()) {
                int flag;
                if (string::to_number(cfg.substr(1uz, cfg.find('|', 1uz) - 1uz), flag) && (flag & 1) == 0)
                    ctx->scope = TargetRange::All;
            }
        } else {
            const int n = static_cast<int>(std::ranges::count(data, ','));

            if (n < 3) {
                index = num = 0;
            } else if (n == 3 && num > 1) {
                int flag;
                if (string::to_number(data.substr(data.rfind(',') + 1), flag) && (flag & 4) != 0)
                    index = 0, num = 1;
            } else if (n != num + 2) {
                return;
            }
        }

        int st = 0, ed = 0;

        if (num != 0) {
            switch (ctx->scope) {
                case TargetRange::Current:
                    st = ed = index;
                    break;
                case TargetRange::All:
                    ed = num;
                    break;
                case TargetRange::Subsequent:
                    st = index, ed = num;
                    break;
                case TargetRange::Preceding:
                    ed = index;
                    break;
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

                            if (!sv.empty() && i >= st && i <= ed && !sv.starts_with('0'))
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

        if (sep != std::string_view::npos)
            result += cfg;

        if (edit->set_object_item_value(ctx->handle, ctx->fx, ctx->prop, result.c_str()))
            logger->info(logger, std::format(L"Updated '{}:{}'", ctx->fx, ctx->prop).c_str());
    });
}
}  // namespace

namespace flow::editor::menu::property::value {
void
init(HOST_APP_TABLE *host, LOG_HANDLE *log_handle, EDIT_HANDLE *edit_handle) {
    logger = log_handle;
    context = edit_handle;

    host->register_object_item_menu_param(
            L"FlowType_K\\現在値で上書き\\全ての区間",
            false,
            nullptr,
            []([[maybe_unused]] void *param, OBJECT_HANDLE handle, const wchar_t *fx, const wchar_t *prop) {
                overwrite_values(TargetRange::All, handle, fx, prop);
            });

    host->register_object_item_menu_param(
            L"FlowType_K\\現在値で上書き\\以前の区間",
            false,
            nullptr,
            []([[maybe_unused]] void *param, OBJECT_HANDLE handle, const wchar_t *fx, const wchar_t *prop) {
                overwrite_values(TargetRange::Preceding, handle, fx, prop);
            });

    host->register_object_item_menu_param(
            L"FlowType_K\\現在値で上書き\\以降の区間",
            false,
            nullptr,
            []([[maybe_unused]] void *param, OBJECT_HANDLE handle, const wchar_t *fx, const wchar_t *prop) {
                overwrite_values(TargetRange::Subsequent, handle, fx, prop);
            });

    host->register_object_item_menu_param(
            L"FlowType_K\\値を反転\\現在の区間",
            false,
            nullptr,
            []([[maybe_unused]] void *param, OBJECT_HANDLE handle, const wchar_t *fx, const wchar_t *prop) {
                invert_values(TargetRange::Current, handle, fx, prop);
            });

    host->register_object_item_menu_param(
            L"FlowType_K\\値を反転\\全ての区間",
            false,
            nullptr,
            []([[maybe_unused]] void *param, OBJECT_HANDLE handle, const wchar_t *fx, const wchar_t *prop) {
                invert_values(TargetRange::All, handle, fx, prop);
            });

    host->register_object_item_menu_param(
            L"FlowType_K\\値を反転\\以前の区間",
            false,
            nullptr,
            []([[maybe_unused]] void *param, OBJECT_HANDLE handle, const wchar_t *fx, const wchar_t *prop) {
                invert_values(TargetRange::Preceding, handle, fx, prop);
            });

    host->register_object_item_menu_param(
            L"FlowType_K\\値を反転\\以降の区間",
            false,
            nullptr,
            []([[maybe_unused]] void *param, OBJECT_HANDLE handle, const wchar_t *fx, const wchar_t *prop) {
                invert_values(TargetRange::Subsequent, handle, fx, prop);
            });
}
}  // namespace flow::editor::menu::property::value
