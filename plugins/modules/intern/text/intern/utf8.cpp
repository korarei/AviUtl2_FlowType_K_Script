#include "../utf8.hpp"

#include <utf8.h>

#include <module2.h>

#include <intern/string.hpp>

namespace {
namespace string = flow::string;

constinit LOG_HANDLE *logger = nullptr;

void
count(SCRIPT_MODULE_PARAM *param) {
    const int n = param->get_param_num();
    if (n != 1 && n != 2) {
        param->set_error("Function call has wrong argument count");
        return;
    }

    const auto text = string::as_string_view(param->get_param_string(0));
    const auto should_pass_ctrl = param->get_param_boolean(1);

    if (text.empty()) {
        param->push_result_int(0);
        return;
    }

    int count = 0;

    auto it = text.begin();
    while (it != text.end()) {
        utf8::utfchar32_t cp = 0;

        try {
            cp = utf8::next(it, text.end());
        } catch (...) {
            param->set_error("Character encoding is unsupported in the text");
            return;
        }

        if (should_pass_ctrl && ((cp <= 0x1f) || (cp == 0x7f) || (cp >= 0x80 && cp <= 0x9f)))
            continue;

        ++count;
    }

    param->push_result_int(count);
}

constinit SCRIPT_MODULE_FUNCTION functions[] = {
        {L"count", count},
        {nullptr, nullptr},
};

constinit SCRIPT_MODULE_TABLE info = {
        .information = L"UTF8@FlowType_K v" VERSION L" by Korarei",
        .functions = functions,
};
}  // namespace

namespace flow::module::text::utf8 {
void
init(HOST_APP_TABLE *host, LOG_HANDLE *handle) {
    logger = handle;

    host->register_script_module_name(&info, L"UTF8@FlowType_K");
}
}  // namespace flow::module::text::utf8
