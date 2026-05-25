#include "../toml.hpp"

#include <string>
#include <vector>

#include <toml++/toml.hpp>

#include <module2.h>

#include <intern/utilities.hpp>

namespace {
namespace string = flow::string;

constinit LOG_HANDLE *logger = nullptr;

constexpr void
parse_motion_effect(SCRIPT_MODULE_PARAM *param) {
    if (param->get_param_num() != 2) {
        param->set_error("Function call has wrong argument count");
        return;
    }

    const auto text = string::as_string_view(param->get_param_string(1));
    if (text.empty())
        return;

    toml::table data;

    try {
        data = toml::parse(text);
    } catch (...) {
        logger->warn(logger, L"TOML syntax is incorrect");
        return;
    }

    for (auto &&[section, node] : data) {
        if (const auto *table = node.as_table(); table != nullptr) {
            std::vector<std::string> keys;
            std::vector<std::string> values;
            keys.reserve(table->size());
            values.reserve(table->size());

            param->push_result_string(std::string(section.str()).c_str());

            for (auto &&[key, value] : *table) {
                if (value.is_string()) {
                    keys.emplace_back(key.str());
                    values.push_back(value.value<std::string>().value_or(""));
                } else if (value.is_boolean()) {
                    keys.emplace_back(key.str());
                    values.push_back(value.as_boolean()->get() ? "1" : "0");
                } else if (value.is_integer()) {
                    keys.emplace_back(key.str());
                    values.push_back(std::to_string(value.value<int64_t>().value_or(0)));
                } else if (value.is_floating_point()) {
                    keys.emplace_back(key.str());
                    values.push_back(std::to_string(value.value<double>().value_or(0.0)));
                }
            }

            auto to_cstrs = [](const std::vector<std::string> &v) {
                std::vector<const char *> p;
                p.reserve(v.size());
                for (auto &s : v) p.push_back(s.c_str());
                return p;
            };

            param->push_result_table_string(
                    to_cstrs(keys).data(), to_cstrs(values).data(), static_cast<int>(keys.size()));
        }
    }
}

void
parse(SCRIPT_MODULE_PARAM *param) {
    const auto tag = string::as_string_view(param->get_param_string(0));

    if (tag == "motion::effect")
        parse_motion_effect(param);
    else
        param->set_error("Unknown tag");
}

constinit SCRIPT_MODULE_FUNCTION functions[] = {
        {L"parse", parse},
        {nullptr, nullptr},
};

constinit SCRIPT_MODULE_TABLE info = {
        .information = L"Toml@FlowType_K v" VERSION L" by Korarei",
        .functions = functions,
};
}  // namespace

namespace flow::modules::toml {
void
init(HOST_APP_TABLE *host, LOG_HANDLE *handle) {
    logger = handle;

    host->register_script_module_name(&info, L"Toml@FlowType_K");
}
}  // namespace flow::modules::toml
