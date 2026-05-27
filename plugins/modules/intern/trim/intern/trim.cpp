#include "../trim.hpp"

#include <execution>
#include <numeric>
#include <ranges>

#include <Eigen/Dense>

#include <module2.h>

namespace {
using Box = Eigen::AlignedBox2i;

constinit LOG_HANDLE *logger = nullptr;

struct RGBA {
    uint8_t r, g, b, a;
};

struct Image {
    Eigen::Vector2i size;
    const RGBA *data;
};

constexpr void
trim(SCRIPT_MODULE_PARAM *param) {
    if (param->get_param_num() != 4) {
        param->set_error("Function call has wrong argument count");
        return;
    }

    const Image img{
            .size = Eigen::Vector2i(param->get_param_int(2), param->get_param_int(3)),
            .data = static_cast<const RGBA *>(param->get_param_data(1)),
    };

    if (img.size.x() <= 0 || img.size.y() <= 0 || img.data == nullptr) {
        param->set_error("Pixel data is null or corrupted");
        return;
    }

    const int threshold = param->get_param_int(0);

    const auto rows = std::ranges::views::iota(0, img.size.y());
    const Box result = std::transform_reduce(
            std::execution::par_unseq,
            rows.begin(),
            rows.end(),
            Box{},
            [](const Box &a, const Box &b) {
                Box tmp = a;
                return tmp.extend(b);
            },
            [&](int y) -> Box {
                Box box{};

                const auto *row = img.data + y * img.size.x();
                for (int x = 0; x < img.size.x(); ++x) {
                    if (row[x].a > threshold)
                        box.extend(Eigen::Vector2i(x, y));
                }

                return box;
            });

    if (result.isEmpty()) {
        param->push_result_int(0);
        param->push_result_int(0);
        param->push_result_int(0);
        param->push_result_int(0);
        return;
    }

    const Eigen::Vector2i size = result.sizes().array() + 1;

    param->push_result_int(result.min().x());
    param->push_result_int(result.min().y());
    param->push_result_int(size.x());
    param->push_result_int(size.y());
}

constinit SCRIPT_MODULE_FUNCTION functions[] = {
        {L"trim", trim},
        {nullptr, nullptr},
};

constinit SCRIPT_MODULE_TABLE info = {
        .information = L"Trim@FlowType_K v" VERSION L" by Korarei",
        .functions = functions,
};
}  // namespace

namespace flow::module::trim {
void
init(HOST_APP_TABLE *host, LOG_HANDLE *handle) {
    logger = handle;

    host->register_script_module_name(&info, L"Trim@FlowType_K");
}
}  // namespace flow::module::trim
