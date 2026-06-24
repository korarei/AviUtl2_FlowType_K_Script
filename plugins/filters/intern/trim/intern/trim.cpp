#include "../trim.hpp"

#include <Eigen/Core>
#include <cstdint>  // IWYU pragma: keep
#include <execution>
#include <memory>
#include <numeric>
#include <ranges>

#include <Eigen/Dense>

#pragma warning(push)
#pragma warning(disable : 4201)  // 非標準の無名構造体 (filter2.h FILTER_ITEM_COLOR)
#include <filter2.h>
#pragma warning(pop)

#include <blit.h>

#ifndef VERSION
#define VERSION L"0.1.0"
#endif

namespace {
using Box = Eigen::AlignedBox2i;

constinit LOG_HANDLE *logger = nullptr;

struct RGBA {
    Eigen::half r, g, b, a;
};

struct Image {
    Eigen::Vector2i size;
    std::unique_ptr<RGBA[]> data;

    Image(int w, int h) : size(w, h), data(std::make_unique_for_overwrite<RGBA[]>(static_cast<size_t>(w) * h)) {}
};

FILTER_ITEM_TRACK threshold = FILTER_ITEM_TRACK(L"Threshold", 0.0, 0.0, 100.0, 0.01);
FILTER_ITEM_GROUP group_anchor = FILTER_ITEM_GROUP(L"Anchor", true);
FILTER_ITEM_SELECT::ITEM anchor_targets[] = {{L"None", 0}, {L"Pivot Point", 1}, {L"Position", 2}, {nullptr, 0}};
FILTER_ITEM_SELECT anchor_target = FILTER_ITEM_SELECT(L"Anchor::Target", 1, anchor_targets);
FILTER_ITEM_CHECK_SECTION anchor_should_overwrite = FILTER_ITEM_CHECK_SECTION(L"Anchor::Overwrite", false, false);
FILTER_ITEM_GROUP group_padding = FILTER_ITEM_GROUP(L"Padding", false);
FILTER_ITEM_TRACK padding_left = FILTER_ITEM_TRACK(L"Padding::Left", 0.0, -1000.0, 1000.0, 1.0);
FILTER_ITEM_TRACK padding_right = FILTER_ITEM_TRACK(L"Padding::Right", 0.0, -1000.0, 1000.0, 1.0);
FILTER_ITEM_TRACK padding_top = FILTER_ITEM_TRACK(L"Padding::Top", 0.0, -1000.0, 1000.0, 1.0);
FILTER_ITEM_TRACK padding_bottom = FILTER_ITEM_TRACK(L"Padding::Bottom", 0.0, -1000.0, 1000.0, 1.0);

void *props[] = {
        &threshold,
        &group_anchor,
        &anchor_target,
        &anchor_should_overwrite,
        &group_padding,
        &padding_left,
        &padding_right,
        &padding_top,
        &padding_bottom,
        nullptr,
};

constexpr bool
trim(FILTER_PROC_VIDEO *video) {
    const Eigen::half t = static_cast<Eigen::half>(threshold.value);

    Image img(video->object->width, video->object->height);

    if (!video->copy_image_resource(L"resource:tmp", L"object")) {
        logger->error(logger, L"Failed to copy buffer");
        return false;
    }

    if (img.size.x() <= 0 || img.size.y() <= 0
        || !video->get_image_resource_data(
                L"object",
                img.data.get(),
                img.size.x(),
                img.size.y(),
                img.size.x() * sizeof(RGBA),
                OUTPUT_PIXEL_FORMAT::HF64)) {
        logger->error(logger, L"Pixel data is null or corrupted");
        return false;
    }

    const auto rows = std::ranges::views::iota(0, img.size.y());
    const Box box = std::transform_reduce(
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

                const auto *row = img.data.get() + y * img.size.x();
                for (int x = 0; x < img.size.x(); ++x) {
                    if (row[x].a > t)
                        box.extend(Eigen::Vector2i(x, y));
                }

                return box;
            });

    if (box.isEmpty())
        return false;

    Eigen::Matrix2i padding;
    padding << static_cast<int>(padding_left.value), static_cast<int>(padding_right.value),
            static_cast<int>(padding_top.value), static_cast<int>(padding_bottom.value);

    Eigen::Vector2i origin = box.min() - padding.col(0);
    const Eigen::Vector2i size = box.sizes() + padding.rowwise().sum() + Eigen::Vector2i::Ones();

    if (size.x() <= 0 || size.y() <= 0)
        return false;

    video->set_image_data(nullptr, size.x(), size.y());
    if (!video->clear_image_resource(L"object", {0, 0, 0, 0})) {
        logger->error(logger, L"Failed to clear buffer");
        return false;
    }

    const wchar_t *resources[] = {L"resource:tmp"};

    if (!video->exec_pixelshader_data(
                g_blit, sizeof(g_blit), L"object", resources, 1, origin.data(), sizeof(origin), nullptr, nullptr)) {
        logger->error(logger, L"Failed to execute pixelshader");
        return false;
    }

    const Eigen::Vector2f delta = origin.cast<float>() + (size - img.size).cast<float>() * 0.5f;

    switch (anchor_target.value) {
        case 1:
            if (anchor_should_overwrite.value)
                video->param->cx = -delta.x(), video->param->cy = -delta.y();
            else
                video->param->cx -= delta.x(), video->param->cy -= delta.y();
            break;
        case 2:
            if (anchor_should_overwrite.value)
                video->param->x = delta.x(), video->param->y = delta.y();
            else
                video->param->x += delta.x(), video->param->y += delta.y();
            break;
        default:
            break;
    }

    return true;
}

constinit FILTER_PLUGIN_TABLE info = {
        .flag = FILTER_PLUGIN_TABLE::FLAG_VIDEO,
        .name = L"Trim@FlowType_K",
        .label = L"Text",
        .information = L"Trim@FlowType_K v" VERSION L" by Korarei",
        .items = props,
        .func_proc_video = trim,
        .func_proc_audio = nullptr,
};
}  // namespace

namespace flow::filter::trim {
void
init(HOST_APP_TABLE *host, LOG_HANDLE *handle) {
    logger = handle;

    host->register_filter_plugin(&info);
}
}  // namespace flow::filter::trim
