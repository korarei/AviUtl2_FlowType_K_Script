#include "../align.hpp"

#include <cstdint>  // IWYU pragma: keep

#pragma warning(push)
#pragma warning(disable : 4201)  // 非標準の無名構造体 (filter2.h FILTER_ITEM_COLOR)
#include <filter2.h>
#pragma warning(pop)

#ifndef VERSION
#define VERSION L"0.1.0"
#endif

namespace {
constinit LOG_HANDLE *logger = nullptr;

auto horizontal = FILTER_ITEM_TRACK(L"Horizontal", 0.0, -1000.0, 1000.0, 0.01);
auto vertical = FILTER_ITEM_TRACK(L"Vertical", 0.0, -1000.0, 1000.0, 0.01);
FILTER_ITEM_SELECT::ITEM targets[] = {{L"Pivot Point", 1}, {L"Position", 2}, {L"Both", 3}, {nullptr, 0}};
auto target = FILTER_ITEM_SELECT(L"Target", 1, targets);
auto should_overwrite = FILTER_ITEM_CHECK_SECTION(L"Overwrite", false, false);

void *props[] = {&horizontal, &vertical, &target, &should_overwrite, nullptr};

constexpr bool
align(FILTER_PROC_VIDEO *video) {
    float cx = 0.0f, cy = 0.0f, ox = 0.0f, oy = 0.0f;
    if (!should_overwrite.value)
        cx = video->param->cx, cy = video->param->cy, ox = video->param->x, oy = video->param->y;

    const float dx = static_cast<float>(horizontal.value) * video->object->width * 0.005f;
    const float dy = static_cast<float>(vertical.value) * video->object->height * 0.005f;

    if ((target.value & 1) != 0)
        video->param->cx = cx + dx, video->param->cy = cy + dy;

    if ((target.value >> 1) != 0)
        video->param->x = ox + dx, video->param->y = oy + dy;

    return true;
}

constinit FILTER_PLUGIN_TABLE info = {
        .flag = FILTER_PLUGIN_TABLE::FLAG_VIDEO,
        .name = L"Align@FlowType_K",
        .label = L"Text",
        .information = L"Align@FlowType_K v" VERSION L" by Korarei",
        .items = props,
        .func_proc_video = align,
        .func_proc_audio = nullptr,
};
}  // namespace

namespace flow::filter::align {
void
init(HOST_APP_TABLE *host, LOG_HANDLE *handle) {
    logger = handle;

    host->register_filter_plugin(&info);
}
}  // namespace flow::filter::align
