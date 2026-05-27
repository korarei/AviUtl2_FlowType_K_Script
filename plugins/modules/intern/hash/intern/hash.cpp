#include "../hash.hpp"

#include <algorithm>
#include <cmath>
#include <cstdint>

#include <module2.h>

namespace {
constinit LOG_HANDLE *logger = nullptr;

/*
The following function is a modified version of pcg4d function
Original implementation by Mark Jarzynski & Marc Olano
https://github.com/markjarzynski/PCG3D/blob/master/LICENSE
*/

inline constexpr void
pcg4d(uint32_t &x, uint32_t &y, uint32_t &z, uint32_t &w) noexcept {
    x = x * 1664525u + 1013904223u;
    y = y * 1664525u + 1013904223u;
    z = z * 1664525u + 1013904223u;
    w = w * 1664525u + 1013904223u;

    x += y * w;
    y += z * x;
    z += x * y;
    w += y * z;

    w ^= w >> 16u;
    x ^= x >> 16u;
    y ^= y >> 16u;
    z ^= z >> 16u;

    x += y * w;
    y += z * x;
    z += x * y;
    w += y * z;
}

constexpr void
hash4d(SCRIPT_MODULE_PARAM *param) {
    const int n = param->get_param_num();
    if (n > 6) {
        param->set_error("Function call has wrong argument count");
        return;
    }

    uint32_t x = static_cast<uint32_t>(param->get_param_double(0));
    uint32_t y = static_cast<uint32_t>(param->get_param_double(1));
    uint32_t z = static_cast<uint32_t>(param->get_param_double(2));
    uint32_t w = static_cast<uint32_t>(param->get_param_double(3));

    pcg4d(x, y, z, w);

    const double hx = x / 4294967296.0;
    const double hy = y / 4294967296.0;
    const double hz = z / 4294967296.0;
    const double hw = w / 4294967296.0;

    switch (n) {
        case 5: {
            const double upper = param->get_param_double(4);
            const double range = upper + 1.0;

            param->push_result_double(std::min(upper, std::floor(range * hx)));
            param->push_result_double(std::min(upper, std::floor(range * hy)));
            param->push_result_double(std::min(upper, std::floor(range * hz)));
            param->push_result_double(std::min(upper, std::floor(range * hw)));
            break;
        }
        case 6: {
            const double lower = param->get_param_double(4);
            const double upper = param->get_param_double(5);
            const double range = upper - lower + 1.0;

            param->push_result_double(std::min(upper, std::floor(range * hx) + lower));
            param->push_result_double(std::min(upper, std::floor(range * hy) + lower));
            param->push_result_double(std::min(upper, std::floor(range * hz) + lower));
            param->push_result_double(std::min(upper, std::floor(range * hw) + lower));
            break;
        }
        default:
            param->push_result_double(hx);
            param->push_result_double(hy);
            param->push_result_double(hz);
            param->push_result_double(hw);
            break;
    }
}

constinit SCRIPT_MODULE_FUNCTION functions[] = {
        {L"hash4d", hash4d},
        {nullptr, nullptr},
};

constinit SCRIPT_MODULE_TABLE info = {
        .information = L"Hash@FlowType_K v" VERSION L" by Korarei",
        .functions = functions,
};
}  // namespace

namespace flow::module::hash {
void
init(HOST_APP_TABLE *host, LOG_HANDLE *handle) {
    logger = handle;

    host->register_script_module_name(&info, L"Hash@FlowType_K");
}
}  // namespace flow::module::hash
