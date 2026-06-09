#pragma once

#include <windows.h>

#include <logger2.h>
#include <plugin2.h>

namespace flow::editor::menu {
void
init(HOST_APP_TABLE *host, LOG_HANDLE *logger, EDIT_HANDLE *context);
}  // namespace flow::editor::menu
