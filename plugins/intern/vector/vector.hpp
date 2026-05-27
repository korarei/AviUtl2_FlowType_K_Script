#pragma once

#include <array>
#include <vector>

#include <Eigen/Dense>

namespace flow::vector {
void
rotate(double t, int mode, double w, double x, double y, double z, std::vector<Eigen::Vector3d> &vectors);

[[nodiscard]] std::array<double, 3uz>
to_euler(double t, int mode, double w, double x, double y, double z);
}  // namespace flow::vector
