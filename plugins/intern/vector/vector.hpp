#pragma once

#include <array>
#include <numbers>
#include <vector>

#include <Eigen/Dense>

namespace flow::vector {
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

void
rotate(double t, int mode, double w, double x, double y, double z, std::vector<Eigen::Vector3d> &vectors);

[[nodiscard]] std::array<double, 3uz>
to_euler(double t, int mode, double w, double x, double y, double z);
}  // namespace flow::vector
