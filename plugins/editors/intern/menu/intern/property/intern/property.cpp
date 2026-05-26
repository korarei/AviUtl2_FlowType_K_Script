#include "../property.hpp"

#include <ranges>
#include <string_view>

#include <intern/utilities.hpp>

namespace {
namespace string = flow::string;

constinit LOG_HANDLE *logger = nullptr;
constinit EDIT_HANDLE *editor = nullptr;

[[nodiscard]] inline constexpr std::wstring
remove_suffix(std::wstring_view s) {
    const auto pos = s.rfind(L':');

    if (pos == std::wstring_view::npos)
        return std::wstring(s);

    return std::wstring(s.substr(0uz, pos));
}

void
copy_value(EDIT_SECTION *edit, OBJECT_HANDLE handle, const wchar_t *fx, const wchar_t *prop) {
    const auto n = edit->get_object_section_num(handle);
    const auto i = edit->get_focus_object_section();
    const auto props = string::as_string_view(edit->get_object_item_value(handle, fx, prop));

    if (props.empty())
        return;

    auto values = props | std::views::split(',');
    auto remaining = values | std::views::drop(1);

    if (remaining.begin() == remaining.end())
        return;

    // 中間点無視の場合はスキップ
    double dummy;
    if (!string::to_number(std::string_view(*(values | std::views::drop(2)).begin()), dummy))
        return;

    const std::string_view src(*(values | std::views::drop(i)).begin());

    const auto result = std::ranges::to<std::string>(
            values | std::views::enumerate | std::views::transform([=](auto &&pair) -> std::string_view {
                const auto [j, v] = pair;
                // トラックバー: [0, n]
                // チェックボックス: [0, n - 1] (これ以降の要素がないのでトラックバーと同一で可)
                return j <= n ? src : std::string_view(v);
            })
            | std::views::join_with(','));

    edit->set_object_item_value(handle, fx, prop, result.c_str());
}

void
copy_value_forward(EDIT_SECTION *edit, OBJECT_HANDLE handle, const wchar_t *fx, const wchar_t *prop) {
    const auto n = edit->get_object_section_num(handle);
    const auto i = edit->get_focus_object_section();
    const auto props = string::as_string_view(edit->get_object_item_value(handle, fx, prop));

    if (props.empty())
        return;

    auto values = props | std::views::split(',');
    auto targets = values | std::views::drop(i);

    if (targets.begin() == targets.end())
        return;

    // 中間点無視の場合はスキップ
    double dummy;
    if (!string::to_number(std::string_view(*(values | std::views::drop(2)).begin()), dummy))
        return;

    const std::string_view src(*targets.begin());

    const auto result = std::ranges::to<std::string>(
            values | std::views::enumerate | std::views::transform([=](auto &&pair) -> std::string_view {
                const auto [j, v] = pair;
                // トラックバー: [0, n]
                // チェックボックス: [0, n - 1] (これ以降の要素がないのでトラックバーと同一で可)
                return j >= i && j <= n ? src : std::string_view(v);
            })
            | std::views::join_with(','));

    edit->set_object_item_value(handle, fx, prop, result.c_str());
}

void
copy_value_backward(EDIT_SECTION *edit, OBJECT_HANDLE handle, const wchar_t *fx, const wchar_t *prop) {
    const auto i = edit->get_focus_object_section();
    const auto props = string::as_string_view(edit->get_object_item_value(handle, fx, prop));

    if (props.empty())
        return;

    auto values = props | std::views::split(',');
    auto remaining = values | std::views::drop(i);

    if (remaining.begin() == remaining.end())
        return;

    // 中間点無視の場合はスキップ
    double dummy;
    if (!string::to_number(std::string_view(*(values | std::views::drop(2)).begin()), dummy))
        return;

    auto targets = values | std::views::take(i);
    if (targets.begin() == targets.end())
        return;

    const std::string_view src(*remaining.begin());

    const auto result = std::ranges::to<std::string>(
            values | std::views::enumerate | std::views::transform([=](auto &&pair) -> std::string_view {
                const auto [j, v] = pair;
                return j < i ? src : std::string_view(v);
            })
            | std::views::join_with(','));

    edit->set_object_item_value(handle, fx, prop, result.c_str());
}

void
invert_values([[maybe_unused]] void *param, OBJECT_HANDLE handle, const wchar_t *fx, const wchar_t *prop) {
    struct Context {
        OBJECT_HANDLE handle;
        const wchar_t *fx;
        const wchar_t *prop;
        int type;
    } ctx{handle, fx, prop, 0};

    if (!editor->enum_effect_item(remove_suffix(fx).c_str(), &ctx, [](void *param, const wchar_t *prop, int type) {
            auto *ctx = static_cast<Context *>(param);

            if (std::wstring_view(ctx->prop) == prop)
                ctx->type = type;
        }))
        return;

    if (ctx.type != EDIT_HANDLE::EFFECT_ITEM_TYPE_INTEGER && ctx.type != EDIT_HANDLE::EFFECT_ITEM_TYPE_NUMBER
        && ctx.type != EDIT_HANDLE::EFFECT_ITEM_TYPE_CHECK)
        return;

    editor->call_edit_section_param(&ctx, [](void *param, EDIT_SECTION *edit) {
        const auto *ctx = static_cast<const Context *>(param);

        auto n = edit->get_object_section_num(ctx->handle);
        const auto props = string::as_string_view(edit->get_object_item_value(ctx->handle, ctx->fx, ctx->prop));

        if (props.empty())
            return;

        auto values = props | std::views::split(',');
        auto remaining = values | std::views::drop(1);

        if (remaining.begin() == remaining.end()) {
            n = 0;
        } else {
            double dummy;
            if (!string::to_number(std::string_view(*(values | std::views::drop(2)).begin()), dummy))
                n = 1;
        }

        std::string result;

        switch (ctx->type) {
            case EDIT_HANDLE::EFFECT_ITEM_TYPE_INTEGER:
            case EDIT_HANDLE::EFFECT_ITEM_TYPE_NUMBER:
                result = std::ranges::to<std::string>(
                        values | std::views::enumerate | std::views::transform([=](auto &&pair) -> std::string {
                            const auto [j, v] = pair;
                            const std::string_view sv(v);

                            if (j <= n && !sv.empty())
                                return sv.starts_with("-") ? std::string(sv.substr(1uz)) : "-" + std::string(sv);

                            return std::string(sv);
                        })
                        | std::views::join_with(','));
                break;
            case EDIT_HANDLE::EFFECT_ITEM_TYPE_CHECK:
                result = std::ranges::to<std::string>(
                        values | std::views::transform([](auto &&v) -> std::string_view {
                            const std::string_view sv(v);
                            return sv == "0" ? "1" : (sv.empty() ? sv : "0");
                        })
                        | std::views::join_with(','));
                break;
        }

        edit->set_object_item_value(ctx->handle, ctx->fx, ctx->prop, result.c_str());
    });
}
}  // namespace

namespace flow::editors::menu::property {
void
init(HOST_APP_TABLE *host, LOG_HANDLE *handle) {
    logger = handle;
    editor = host->create_edit_handle();

    host->register_object_item_menu(L"FlowType_K\\すべての区間へ値をコピー", false, copy_value);
    host->register_object_item_menu(L"FlowType_K\\これ以降の区間へ値をコピー", false, copy_value_forward);
    host->register_object_item_menu(L"FlowType_K\\これ以前の区間へ値をコピー", false, copy_value_backward);
    host->register_object_item_menu_param(L"FlowType_K\\すべての区間の値を反転", false, nullptr, invert_values);
}
}  // namespace flow::editors::menu::property
