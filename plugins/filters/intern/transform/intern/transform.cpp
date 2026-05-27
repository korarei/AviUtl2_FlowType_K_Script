#include "../transform.hpp"

#include <cmath>
#include <cstdint>  // IWYU pragma: keep

#pragma warning(push)
#pragma warning(disable : 4201)  // 非標準の無名構造体 (filter2.h FILTER_ITEM_COLOR)
#include <filter2.h>
#pragma warning(pop)

#include <intern/vector/vector.hpp>

namespace {
namespace vector = flow::vector;

constinit LOG_HANDLE *logger = nullptr;

auto group_pivot = FILTER_ITEM_GROUP(L"Pivot Point", true);
auto pivot_x = FILTER_ITEM_TRACK(L"Pivot::X", 0.0, -100000.0, 100000.0, 0.01);
auto pivot_y = FILTER_ITEM_TRACK(L"Pivot::Y", 0.0, -100000.0, 100000.0, 0.01);
auto pivot_z = FILTER_ITEM_TRACK(L"Pivot::Z", 0.0, -100000.0, 100000.0, 0.01);
FILTER_ITEM_TRACK *pivot[] = {&pivot_x, &pivot_y, &pivot_z, nullptr};
auto trackgroup_pivot = FILTER_ITEM_TRACK_GROUP(L"Group::Pivot", pivot);
auto group_position = FILTER_ITEM_GROUP(L"Position", true);
auto position_x = FILTER_ITEM_TRACK(L"Position::X", 0.0, -100000.0, 100000.0, 0.01);
auto position_y = FILTER_ITEM_TRACK(L"Position::Y", 0.0, -100000.0, 100000.0, 0.01);
auto position_z = FILTER_ITEM_TRACK(L"Position::Z", 0.0, -100000.0, 100000.0, 0.01);
FILTER_ITEM_TRACK *position[] = {&position_x, &position_y, &position_z, nullptr};
auto trackgroup_position = FILTER_ITEM_TRACK_GROUP(L"Group::Position", position);
auto group_rotation = FILTER_ITEM_GROUP(L"Rotation", true);
auto rotation_w = FILTER_ITEM_TRACK(L"Rotation::W", 0.0, -3600.0, 3600.0, 0.01);
auto rotation_x = FILTER_ITEM_TRACK(L"Rotation::X", 0.0, -3600.0, 3600.0, 0.01);
auto rotation_y = FILTER_ITEM_TRACK(L"Rotation::Y", 0.0, -3600.0, 3600.0, 0.01);
auto rotation_z = FILTER_ITEM_TRACK(L"Rotation::Z", 0.0, -3600.0, 3600.0, 0.01);
FILTER_ITEM_SELECT::ITEM rotation_modes[] = {
        {L"Quaternion", 0},
        {L"Axis Angle", 1},
        {L"XYZ Euler", 5},
        {L"XZY Euler", 7},
        {L"YXZ Euler", 11},
        {L"YZX Euler", 15},
        {L"ZXY Euler", 19},
        {L"ZYX Euler", 21},
        {nullptr, 0},
};
auto rotation_mode = FILTER_ITEM_SELECT(L"Rotation::Mode", 21, rotation_modes);
FILTER_ITEM_TRACK *rotation[] = {&rotation_x, &rotation_y, &rotation_z, nullptr};
auto trackgroup_rotation = FILTER_ITEM_TRACK_GROUP(L"Group::Rotation", rotation);
auto group_scale = FILTER_ITEM_GROUP(L"Scale", true);
auto scale_x = FILTER_ITEM_TRACK(L"Scale::X", 100.0, -10000.0, 10000.0, 0.01);
auto scale_y = FILTER_ITEM_TRACK(L"Scale::Y", 100.0, -10000.0, 10000.0, 0.01);
auto scale_z = FILTER_ITEM_TRACK(L"Scale::Z", 100.0, -10000.0, 10000.0, 0.01);
FILTER_ITEM_TRACK *scale[] = {&scale_x, &scale_y, &scale_z, nullptr};
auto trackgroup_scale = FILTER_ITEM_TRACK_GROUP(L"Group::Scale", scale);
auto group_target = FILTER_ITEM_GROUP(L"Target", true);
auto target_local_space = FILTER_ITEM_CHECK_SECTION(L"Target::Local Space", true);
auto target_world_space = FILTER_ITEM_CHECK_SECTION(L"Target::World Space", false);
auto group_additional_options = FILTER_ITEM_GROUP(L"Additional Options", false);
auto influence = FILTER_ITEM_TRACK(L"Influence", 100.0, 0.0, 100.0, 0.01);

void *props[] = {
        &group_pivot,
        &pivot_x,
        &pivot_y,
        &pivot_z,
        &trackgroup_pivot,
        &group_position,
        &position_x,
        &position_y,
        &position_z,
        &trackgroup_position,
        &group_rotation,
        &rotation_w,
        &rotation_x,
        &rotation_y,
        &rotation_z,
        &rotation_mode,
        &trackgroup_rotation,
        &group_scale,
        &scale_x,
        &scale_y,
        &scale_z,
        &trackgroup_scale,
        &group_target,
        &target_local_space,
        &target_world_space,
        &group_additional_options,
        &influence,
        nullptr,
};

constexpr bool
transform(FILTER_PROC_VIDEO *video) {
    const double t = influence.value * 0.01;
    const double sx = scale_x.value * 0.01, sy = scale_y.value * 0.01, sz = scale_z.value * 0.01;
    double rw = rotation_w.value, rx = rotation_x.value, ry = rotation_y.value, rz = rotation_z.value;

    if (rotation_mode.value == 1)
        rw = vector::to_rad(rw);
    else if (rotation_mode.value >= 5 && rotation_mode.value <= 21)
        rx = vector::to_rad(rx), ry = vector::to_rad(ry), rz = vector::to_rad(rz);

    if (target_world_space.value) {
        const Eigen::Vector3d v(
                (video->param->x - pivot_x.value * t) * (1.0 + (sx - 1.0) * t),
                (video->param->y - pivot_y.value * t) * (1.0 + (sy - 1.0) * t),
                (video->param->z - pivot_z.value * t) * (1.0 + (sz - 1.0) * t));

        std::vector<Eigen::Vector3d> vectors{v};

        vector::rotate(t, rotation_mode.value, rw, rx, ry, rz, vectors);

        video->param->x = static_cast<float>(vectors[0].x());
        video->param->y = static_cast<float>(vectors[0].y());
        video->param->z = static_cast<float>(vectors[0].z());
    }

    if (target_local_space.value) {
        const auto angle = vector::to_euler(t, rotation_mode.value, rw, rx, ry, rz);

        video->param->cx += static_cast<float>(pivot_x.value * t);
        video->param->cy += static_cast<float>(pivot_y.value * t);
        video->param->cz += static_cast<float>(pivot_z.value * t);

        video->param->rx += static_cast<float>(vector::to_deg(angle[0]));
        video->param->ry += static_cast<float>(vector::to_deg(angle[1]));
        video->param->rz += static_cast<float>(vector::to_deg(angle[2]));

        video->param->sx = static_cast<float>(std::lerp(video->param->sx, video->param->sx * sx, t));
        video->param->sy = static_cast<float>(std::lerp(video->param->sy, video->param->sy * sy, t));
        video->param->sz = static_cast<float>(std::lerp(video->param->sz, video->param->sz * sz, t));
    }

    if (target_local_space.value || target_world_space.value) {
        video->param->x += static_cast<float>(position_x.value * t);
        video->param->y += static_cast<float>(position_y.value * t);
        video->param->z += static_cast<float>(position_z.value * t);
    }

    return true;
}

constinit FILTER_PLUGIN_TABLE info = {
        .flag = FILTER_PLUGIN_TABLE::FLAG_VIDEO,
        .name = L"Transform@FlowType_K",
        .label = L"Text",
        .information = L"Transform@FlowType_K v" VERSION L" by Korarei",
        .items = props,
        .func_proc_video = transform,
        .func_proc_audio = nullptr,
};
}  // namespace

namespace flow::filter::transform {
void
init(HOST_APP_TABLE *host, LOG_HANDLE *handle) {
    logger = handle;

    host->register_filter_plugin(&info);
}
}  // namespace flow::filter::transform
