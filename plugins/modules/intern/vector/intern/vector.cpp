#include "../vector.hpp"

#include <module2.h>

#include <intern/vector/vector.hpp>

#ifndef VERSION
#define VERSION L"0.1.0"
#endif

namespace {
namespace vector = flow::vector;

constinit LOG_HANDLE *logger = nullptr;

constexpr void
rotate(SCRIPT_MODULE_PARAM *param) {
    const auto n = param->get_param_num();

    if (n == 6) {
        const auto t = param->get_param_double(0);
        const auto mode = param->get_param_int(1);
        auto w = param->get_param_double(2);
        auto x = param->get_param_double(3);
        auto y = param->get_param_double(4);
        auto z = param->get_param_double(5);

        if (mode == 1)
            w = vector::to_rad(w);
        else if (mode >= 5 && mode <= 21)
            x = vector::to_rad(x), y = vector::to_rad(y), z = vector::to_rad(z);

        try {
            auto angle = vector::to_euler(t, mode, w, x, y, z);

            param->push_result_double(vector::to_deg(angle[0]));
            param->push_result_double(vector::to_deg(angle[1]));
            param->push_result_double(vector::to_deg(angle[2]));
        } catch (const std::runtime_error &e) {
            param->set_error(e.what());
        }
    } else if (n >= 7) {
        const auto t = param->get_param_double(0);
        const auto mode = param->get_param_int(1);
        auto w = param->get_param_double(2);
        auto x = param->get_param_double(3);
        auto y = param->get_param_double(4);
        auto z = param->get_param_double(5);

        const int size = param->get_param_num() - 6;
        std::vector<Eigen::Vector3d> vectors(size);
        for (int i = 0; i < size; ++i) {
            const int j = i + 6;
            vectors[i] = {
                    param->get_param_array_double(j, 0),
                    param->get_param_array_double(j, 1),
                    param->get_param_array_double(j, 2),
            };
        }

        if (mode == 1)
            w = vector::to_rad(w);
        else if (mode >= 5 && mode <= 21)
            x = vector::to_rad(x), y = vector::to_rad(y), z = vector::to_rad(z);

        try {
            vector::rotate(t, mode, w, x, y, z, vectors);
        } catch (const std::runtime_error &e) {
            param->set_error(e.what());
            return;
        }

        for (auto &v : vectors) param->push_result_array_double(v.data(), 3);
    } else {
        param->set_error("Function call has wrong argument count");
    }
}

constinit SCRIPT_MODULE_FUNCTION functions[] = {
        {L"rotate", rotate},
        {nullptr, nullptr},
};

constinit SCRIPT_MODULE_TABLE info = {
        .information = L"Vector@FlowType_K v" VERSION L" by Korarei",
        .functions = functions,
};
}  // namespace

namespace flow::module::vector {
void
init(HOST_APP_TABLE *host, LOG_HANDLE *handle) {
    logger = handle;

    host->register_script_module_name(&info, L"Vector@FlowType_K");
}
}  // namespace flow::module::vector
