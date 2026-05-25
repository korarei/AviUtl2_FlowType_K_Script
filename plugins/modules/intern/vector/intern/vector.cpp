#include "../vector.hpp"

#include <array>
#include <numbers>
#include <vector>

#include <Eigen/Dense>
#include <Eigen/Geometry>

#include <module2.h>

namespace {
constinit LOG_HANDLE *logger = nullptr;
const Eigen::Quaterniond q0(1.0, 0.0, 0.0, 0.0);

[[nodiscard]] inline constexpr double
to_rad(double deg) noexcept {
    constexpr double f = std::numbers::pi / 180.0;
    return deg * f;
}

[[nodiscard]] inline constexpr double
to_deg(double rad) noexcept {
    constexpr double f = 180.0 / std::numbers::pi;
    return rad * f;
}

[[nodiscard]] inline constexpr Eigen::Matrix3d
to_rotation_matrix(int mode, const std::array<double, 3> &angle) noexcept {
    const std::array<int, 3> order{mode / 9, (mode / 3) % 3, mode % 3};
    const std::array<Eigen::Vector3d, 3> axis = {
            Eigen::Vector3d::UnitX(),
            Eigen::Vector3d::UnitY(),
            Eigen::Vector3d::UnitZ(),
    };

    return (Eigen::AngleAxisd(to_rad(angle[order[2]]), axis[order[2]])
            * Eigen::AngleAxisd(to_rad(angle[order[1]]), axis[order[1]])
            * Eigen::AngleAxisd(to_rad(angle[order[0]]), axis[order[0]]))
            .toRotationMatrix();
}

constexpr void
rotate_vector(SCRIPT_MODULE_PARAM *param) {
    const auto t = param->get_param_double(0);
    const auto mode = param->get_param_int(1);
    const auto w = param->get_param_double(2);
    const auto x = param->get_param_double(3);
    const auto y = param->get_param_double(4);
    const auto z = param->get_param_double(5);

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

    if (mode == 0) {
        const auto q = q0.slerp(std::abs(t), Eigen::Quaterniond(w, x, y, z).normalized());

        for (auto &v : vectors) {
            v = q * v;
            param->push_result_array_double(v.data(), 3);
        }
    } else if (mode == 1) {
        const Eigen::AngleAxisd aa(to_rad(w * t), Eigen::Vector3d(x, y, z).normalized());

        for (auto &v : vectors) {
            v = aa * v;
            param->push_result_array_double(v.data(), 3);
        }
    } else if (mode >= 5 && mode <= 21) {
        const auto rm = to_rotation_matrix(mode, {x * t, y * t, z * t});

        for (auto &v : vectors) {
            v = rm * v;
            param->push_result_array_double(v.data(), 3);
        }
    } else {
        param->set_error("Selected rotation mode is unsupported");
    }
}

constexpr void
to_euler(SCRIPT_MODULE_PARAM *param) {
    const auto t = param->get_param_double(0);
    const auto mode = param->get_param_int(1);
    const auto w = param->get_param_double(2);
    const auto x = param->get_param_double(3);
    const auto y = param->get_param_double(4);
    const auto z = param->get_param_double(5);

    if (mode == 0) {
        const auto q = q0.slerp(std::abs(t), Eigen::Quaterniond(w, x, y, z).normalized());
        const Eigen::Vector3d euler = q.toRotationMatrix().canonicalEulerAngles(0, 1, 2);
        param->push_result_double(to_deg(euler.x()));
        param->push_result_double(to_deg(euler.y()));
        param->push_result_double(to_deg(euler.z()));
    } else if (mode == 1) {
        const Eigen::AngleAxisd aa(to_rad(w * t), Eigen::Vector3d(x, y, z).normalized());
        const Eigen::Vector3d euler = aa.toRotationMatrix().canonicalEulerAngles(0, 1, 2);
        param->push_result_double(to_deg(euler.x()));
        param->push_result_double(to_deg(euler.y()));
        param->push_result_double(to_deg(euler.z()));
    } else if (mode >= 5 && mode <= 21) {
        const auto rm = to_rotation_matrix(mode, {x * t, y * t, z * t});
        const Eigen::Vector3d euler = rm.canonicalEulerAngles(0, 1, 2);
        param->push_result_double(to_deg(euler.x()));
        param->push_result_double(to_deg(euler.y()));
        param->push_result_double(to_deg(euler.z()));
    } else {
        param->set_error("Selected rotation mode is unsupported");
    }
}

constexpr void
rotate(SCRIPT_MODULE_PARAM *param) {
    const auto n = param->get_param_num();

    if (n == 6)
        to_euler(param);
    else if (n >= 7)
        rotate_vector(param);
    else
        param->set_error("Function call has wrong argument count");
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

namespace flow::modules::vector {
void
init(HOST_APP_TABLE *host, LOG_HANDLE *handle) {
    logger = handle;

    host->register_script_module_name(&info, L"Vector@FlowType_K");
}
}  // namespace flow::modules::vector
