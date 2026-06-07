#include "property.hpp"

#include <module2.h>

#include <intern/alias/alias.hpp>
#include <intern/string.hpp>

namespace {
namespace string = flow::string;

using Object = flow::alias::Object;

constinit LOG_HANDLE *logger = nullptr;

void
is_text(SCRIPT_MODULE_PARAM *param) {
    if (param->get_param_num() != 2) {
        param->set_error("Function call has wrong argument count");
        return;
    }

    const auto layer = param->get_param_int(0) - 1;
    const auto frame = param->get_param_int(1);

    const auto handle = param->edit->find_object(layer, frame);
    if (handle == nullptr) {
        param->set_error("Object not found");
        return;
    }

    if (param->edit->get_object_item_value(handle, L"テキスト", L"B") != nullptr)
        param->push_result_data(handle);
}

void
property(SCRIPT_MODULE_PARAM *param) {
    if (param->get_param_num() != 2) {
        param->set_error("Function call has wrong argument count");
        return;
    }

    const auto handle = param->get_param_data(0);
    const auto frame = param->get_param_double(1);

    if (handle == nullptr) {
        param->set_error("Invalid object handle");
        return;
    }

    const auto alias = param->edit->get_object_alias(handle);
    if (alias == nullptr) {
        param->set_error("Failed to get object alias");
        return;
    }

    const Object object(alias);
    const auto text = object.get(0);
    if (text.get("effect.name") != "テキスト")
        return;

    double value;

    if (param->edit->get_object_track_value(handle, L"テキスト", L"サイズ", frame, &value))
        param->push_result_double(value);
    else
        param->push_result_double(0.0);

    if (param->edit->get_object_track_value(handle, L"テキスト", L"字間", frame, &value))
        param->push_result_double(value);
    else
        param->push_result_double(0.0);

    if (param->edit->get_object_track_value(handle, L"テキスト", L"行間", frame, &value))
        param->push_result_double(value);
    else
        param->push_result_double(0.0);

    if (param->edit->get_object_track_value(handle, L"テキスト", L"表示速度", frame, &value))
        param->push_result_double(value);
    else
        param->push_result_double(0.0);

    param->push_result_string(std::string(text.get("フォント")).c_str());

    int color;

    if (string::to_number(text.get("文字色"), color, 16))
        param->push_result_int(color);
    else
        param->push_result_int(0x000000);

    if (string::to_number(text.get("影・縁色"), color, 16))
        param->push_result_int(color);
    else
        param->push_result_int(0x000000);

    auto style = text.get("文字装飾");

    if (style.starts_with("標")) {
        param->push_result_int(0);
    } else if (style.starts_with("影")) {
        if (style.ends_with("薄)"))
            param->push_result_int(2);
        else
            param->push_result_int(1);
    } else if (style.starts_with("縁")) {
        if (style.ends_with("字")) {
            param->push_result_int(3);
        } else {
            style.remove_suffix(1uz);
            if (style.ends_with("角"))
                param->push_result_int(6);
            else if (style.ends_with("太"))
                param->push_result_int(5);
            else if (style.ends_with("細"))
                param->push_result_int(4);
            else
                param->push_result_int(3);
        }
    } else {
        param->push_result_int(0);
    }

    auto alignment = text.get("文字揃え");
    alignment.remove_suffix(1uz);

    if (alignment.starts_with("縦")) {
        alignment.remove_prefix(7uz);
        const int y = alignment.ends_with("左") ? 2 : alignment.ends_with("中") ? 1 : 0;
        const int x = alignment.starts_with("下") ? 2 : alignment.starts_with("中") ? 1 : 0;
        param->push_result_int(x + y * 3 + 9);
    } else {
        const int y = alignment.ends_with("下") ? 2 : alignment.ends_with("中") ? 1 : 0;
        const int x = alignment.starts_with("右") ? 2 : alignment.starts_with("中") ? 1 : 0;
        param->push_result_int(x + y * 3);
    }

    bool check;

    if (param->edit->get_object_check_value(handle, L"テキスト", L"B", static_cast<int>(frame), &check))
        param->push_result_boolean(check);
    else
        param->push_result_boolean(false);

    if (param->edit->get_object_check_value(handle, L"テキスト", L"I", static_cast<int>(frame), &check))
        param->push_result_boolean(check);
    else
        param->push_result_boolean(false);
}

void
content(SCRIPT_MODULE_PARAM *param) {
    if (param->get_param_num() != 1) {
        param->set_error("Function call has wrong argument count");
        return;
    }

    const auto handle = param->get_param_data(0);
    if (handle == nullptr) {
        param->set_error("Invalid object handle");
        return;
    }

    const auto alias = param->edit->get_object_alias(handle);
    if (alias == nullptr) {
        param->set_error("Failed to get object alias");
        return;
    }

    const Object object(alias);
    const auto text = object.get(0);
    if (text.get("effect.name") != "テキスト")
        return;

    param->push_result_string(Object::Effect::unescape(text.get("テキスト")).c_str());
}

constinit SCRIPT_MODULE_FUNCTION functions[] = {
        {L"is_text", is_text},
        {L"property", property},
        {L"content", content},
        {nullptr, nullptr},
};

constinit SCRIPT_MODULE_TABLE info = {
        .information = L"Text@FlowType_K v" VERSION L" by Korarei",
        .functions = functions,
};
}  // namespace

namespace flow::module::text::property {
void
init(HOST_APP_TABLE *host, LOG_HANDLE *handle) {
    logger = handle;

    host->register_script_module_name(&info, L"Text@FlowType_K");
}
}  // namespace flow::module::text::property
