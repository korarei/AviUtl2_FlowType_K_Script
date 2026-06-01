#include "../deform.hpp"

#include <array>
#include <cmath>
#include <cstdint>  // IWYU pragma: keep

#pragma warning(push)
#pragma warning(disable : 4201)  // 非標準の無名構造体 (filter2.h FILTER_ITEM_COLOR)
#include <filter2.h>
#pragma warning(pop)

#include <intern/vector/vector.hpp>

#ifndef VERSION
#define VERSION L"0.1.0"
#endif

namespace {
namespace vector = flow::vector;

constinit LOG_HANDLE *logger = nullptr;

auto group_pivot = FILTER_ITEM_GROUP(L"Pivot Point", true);
auto pivot_x = FILTER_ITEM_TRACK(L"Pivot::X", 0.0, -100000.0, 100000.0, 0.01);
auto pivot_y = FILTER_ITEM_TRACK(L"Pivot::Y", 0.0, -100000.0, 100000.0, 0.01);
FILTER_ITEM_TRACK *pivot_list[] = {&pivot_x, &pivot_y, nullptr};
auto trackgroup_pivot = FILTER_ITEM_TRACK_GROUP(L"Group::Pivot", pivot_list);
auto group_position = FILTER_ITEM_GROUP(L"Position", true);
auto position_x = FILTER_ITEM_TRACK(L"Position::X", 0.0, -100000.0, 100000.0, 0.01);
auto position_y = FILTER_ITEM_TRACK(L"Position::Y", 0.0, -100000.0, 100000.0, 0.01);
FILTER_ITEM_TRACK *position_list[] = {&position_x, &position_y, nullptr};
auto trackgroup_position = FILTER_ITEM_TRACK_GROUP(L"Group::Position", position_list);
auto group_scale = FILTER_ITEM_GROUP(L"Scale", true);
auto scale_x = FILTER_ITEM_TRACK(L"Scale::X", 100.0, -10000.0, 10000.0, 0.01);
auto scale_y = FILTER_ITEM_TRACK(L"Scale::Y", 100.0, -10000.0, 10000.0, 0.01);
FILTER_ITEM_TRACK *scale_list[] = {&scale_x, &scale_y, nullptr};
auto trackgroup_scale = FILTER_ITEM_TRACK_GROUP(L"Group::Scale", scale_list);
auto group_skew = FILTER_ITEM_GROUP(L"Skew", true);
auto skew_angle = FILTER_ITEM_TRACK(L"Skew::Angle", 0.0, -70.0, 70.0, 0.01);
auto skew_axis = FILTER_ITEM_TRACK(L"Skew::Axis", 0.0, -3600.0, 3600.0, 0.01);
auto group_end = FILTER_ITEM_GROUP(L"", true);
auto rotation = FILTER_ITEM_TRACK(L"Rotation", 0.0, -3600.0, 3600.0, 0.01);
auto opacity = FILTER_ITEM_TRACK(L"Opacity", 100.0, 0.0, 100.0, 0.01);
FILTER_ITEM_SELECT::ITEM sampling_modes[] = {{L"Nearest Neighbor", 0}, {L"Bilinear", 1}, {nullptr, 0}};
auto sampling = FILTER_ITEM_SELECT(L"Sampling", 1, sampling_modes);

void *props[] = {
        &group_pivot,         &pivot_x,     &pivot_y,  &trackgroup_pivot, &group_position,   &position_x, &position_y,
        &trackgroup_position, &group_scale, &scale_x,  &scale_y,          &trackgroup_scale, &group_skew, &skew_angle,
        &skew_axis,           &group_end,   &rotation, &opacity,          &sampling,         nullptr,
};

constexpr bool
deform(FILTER_PROC_VIDEO *video) {
    const float alpha = static_cast<float>(opacity.value) * 0.01f;

    Eigen::Transform<float, 2, Eigen::Affine> t = Eigen::Transform<float, 2, Eigen::Affine>::Identity();

    t.translate(Eigen::Vector2f(static_cast<float>(position_x.value), static_cast<float>(position_y.value)));
    t.rotate(static_cast<float>(vector::to_rad(rotation.value)));

    const float axis = static_cast<float>(vector::to_rad(skew_axis.value));
    Eigen::Matrix2f shear;
    shear << 1.0f, 0.0f, std::tan(static_cast<float>(vector::to_rad(skew_angle.value))), 1.0f;

    t.rotate(-axis);
    t.linear() *= shear;
    t.rotate(axis);

    t.scale(Eigen::Vector2f(scale_x.value * 0.01f, scale_y.value * 0.01f));
    t.translate(Eigen::Vector2f(-static_cast<float>(pivot_x.value), -static_cast<float>(pivot_y.value)));

    const float cx = video->object->width * 0.5f, cy = video->object->height * 0.5f;

    Eigen::Matrix<float, 2, 4> verts;
    verts << -cx, cx, cx, -cx, -cy, -cy, cy, cy;

    verts = t * verts;

    const int w = static_cast<int>(std::ceil(verts.row(0).cwiseAbs().maxCoeff())) * 2;
    const int h = static_cast<int>(std::ceil(verts.row(1).cwiseAbs().maxCoeff())) * 2;

    if (w <= 0 || h <= 0)
        return false;

    video->set_sampler_mode(sampling.value == 0 ? SAMPLER_MODE::DOT : SAMPLER_MODE::CLAMP);

    video->create_image_resource(L"resource:result", nullptr, w, h);

    if (!video->clear_image_resource(L"resource:result", {0, 0, 0, 0})) {
        logger->error(logger, L"Failed to clear buffer");
        return false;
    }

    std::array<VERTEX_TEXTURE, 4> vertices{};
    vertices[0] = {verts(0, 0), verts(1, 0), 0.0f, 0.0f, 0.0f, alpha};
    vertices[1] = {verts(0, 1), verts(1, 1), 0.0f, 1.0f, 0.0f, alpha};
    vertices[2] = {verts(0, 2), verts(1, 2), 0.0f, 1.0f, 1.0f, alpha};
    vertices[3] = {verts(0, 3), verts(1, 3), 0.0f, 0.0f, 1.0f, alpha};

    if (!video->draw_poly_to_resource(L"resource:result", VERTEX_TYPE::QUAD_TEXTURE, vertices.data(), 4, L"object")) {
        logger->error(logger, L"Failed to draw image");
        return false;
    }

    if (!video->copy_image_resource(L"object", L"resource:result")) {
        logger->error(logger, L"Failed to copy buffer");
        return false;
    }

    if (video->object->num > 1) {
        Eigen::Vector2f pos(video->param->x, video->param->y);
        pos = t * pos;
        video->param->x = pos.x();
        video->param->y = pos.y();
    }

    return true;
}

constinit FILTER_PLUGIN_TABLE info = {
        .flag = FILTER_PLUGIN_TABLE::FLAG_VIDEO,
        .name = L"Deform@FlowType_K",
        .label = L"Text",
        .information = L"Deform@FlowType_K v" VERSION L" by Korarei",
        .items = props,
        .func_proc_video = deform,
        .func_proc_audio = nullptr,
};
}  // namespace

namespace flow::filter::deform {
void
init(HOST_APP_TABLE *host, LOG_HANDLE *handle) {
    logger = handle;

    host->register_filter_plugin(&info);
}
}  // namespace flow::filter::deform
