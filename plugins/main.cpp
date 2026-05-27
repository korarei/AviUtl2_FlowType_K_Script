#include <windows.h>

#include <logger2.h>
#include <plugin2.h>

#include "editors/editor.hpp"
#include "intern/font/font.hpp"
#include "modules/module.hpp"

#ifndef VERSION
#define VERSION L"0.1.0"
#endif

#ifndef REQUIRES_AVIUTL2
#define REQUIRES_AVIUTL2 2000100u
#endif

namespace {
constinit LOG_HANDLE *logger = nullptr;

constinit COMMON_PLUGIN_TABLE info = {
        .name = L"FlowType_K",
        .information = L"FlowType_K v" VERSION L" by Korarei",
};
}  // namespace

extern "C" {
void
InitializeLogger(LOG_HANDLE *handle) {
    logger = handle;
}

DWORD
RequiredVersion() { return REQUIRES_AVIUTL2; }

bool
InitializePlugin(DWORD version) {
    return version >= RequiredVersion();
}

void
UninitializePlugin() {
    flow::module::deinit();
    flow::font::FontCache::reset();
    flow::font::DWrite::reset();
}

COMMON_PLUGIN_TABLE *
GetCommonPluginTable() {
    return &info;
};

void
RegisterPlugin(HOST_APP_TABLE *host) {
    flow::module::init(host, logger);
    flow::editor::init(host, logger);
}
}
