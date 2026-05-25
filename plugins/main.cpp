#include <windows.h>

#include <logger2.h>
#include <plugin2.h>

#include "editors/editor.hpp"
#include "intern/font/font.hpp"
#include "modules/module.hpp"

#ifndef VERSION
#define VERSION L"0.1.0"
#endif

namespace {
using namespace flow;

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
RequiredVersion() { return 2004500u; }

bool
InitializePlugin(DWORD version) {
    return version >= RequiredVersion();
}

void
UninitializePlugin() {
    flow::modules::deinit();
    flow::font::FontCache::reset();
    flow::font::DWrite::reset();
}

COMMON_PLUGIN_TABLE *
GetCommonPluginTable() {
    return &info;
};

void
RegisterPlugin(HOST_APP_TABLE *host) {
    flow::modules::init(host, logger);
    flow::editors::init(host, logger);
}
}
