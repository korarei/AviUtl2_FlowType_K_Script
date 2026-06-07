#include "../vector.hpp"

#include <Eigen/Dense>

namespace {
const Eigen::Quaterniond q0(1.0, 0.0, 0.0, 0.0);

[[nodiscard]] inline constexpr Eigen::Matrix3d
to_rotation_matrix(int mode, const std::array<double, 3> &angle) noexcept {
    const std::array<int, 3> order{mode / 9, (mode / 3) % 3, mode % 3};
    const std::array<Eigen::Vector3d, 3> axis = {
            Eigen::Vector3d::UnitX(),
            Eigen::Vector3d::UnitY(),
            Eigen::Vector3d::UnitZ(),
    };

    return (Eigen::AngleAxisd(angle[order[2]], axis[order[2]]) * Eigen::AngleAxisd(angle[order[1]], axis[order[1]])
            * Eigen::AngleAxisd(angle[order[0]], axis[order[0]]))
            .toRotationMatrix();
}
}  // namespace

namespace flow::vector {
void
rotate(double t, int mode, Unit unit, double w, double x, double y, double z, std::span<Eigen::Vector3d> vectors) {
    if (mode == 0) {
        const auto q = q0.slerp(std::abs(t), Eigen::Quaterniond(w, x, y, z).normalized());

        for (auto &v : vectors) v = q * v;
    } else if (mode == 1) {
        if (unit == Unit::Degree)
            w = to_rad(w);

        const Eigen::AngleAxisd aa(w * t, Eigen::Vector3d(x, y, z).normalized());

        for (auto &v : vectors) v = aa * v;
    } else if (mode >= 5 && mode <= 21) {
        if (unit == Unit::Degree)
            x = to_rad(x), y = to_rad(y), z = to_rad(z);

        const auto rm = to_rotation_matrix(mode, {x * t, y * t, z * t});

        for (auto &v : vectors) v = rm * v;
    } else {
        throw std::runtime_error("Selected rotation mode is unsupported");
    }
}

std::array<double, 3uz>
to_euler(double t, int mode, Unit unit, double w, double x, double y, double z) {
    std::array<double, 3uz> result;

    if (mode == 0) {
        const auto q = q0.slerp(std::abs(t), Eigen::Quaterniond(w, x, y, z).normalized());
        const Eigen::Vector3d euler = q.toRotationMatrix().canonicalEulerAngles(0, 1, 2);
        result = {euler.x(), euler.y(), euler.z()};
    } else if (mode == 1) {
        if (unit == Unit::Degree)
            w = to_rad(w);

        const Eigen::AngleAxisd aa(w * t, Eigen::Vector3d(x, y, z).normalized());
        const Eigen::Vector3d euler = aa.toRotationMatrix().canonicalEulerAngles(0, 1, 2);
        result = {euler.x(), euler.y(), euler.z()};
    } else if (mode >= 5 && mode <= 21) {
        if (unit == Unit::Degree)
            x = to_rad(x), y = to_rad(y), z = to_rad(z);

        const auto rm = to_rotation_matrix(mode, {x * t, y * t, z * t});
        const Eigen::Vector3d euler = rm.canonicalEulerAngles(0, 1, 2);
        result = {euler.x(), euler.y(), euler.z()};
    } else {
        throw std::runtime_error("Selected rotation mode is unsupported");
    }

    if (unit == Unit::Degree)
        result[0] = to_deg(result[0]), result[1] = to_deg(result[1]), result[2] = to_deg(result[2]);

    return result;
}
}  // namespace flow::vector
