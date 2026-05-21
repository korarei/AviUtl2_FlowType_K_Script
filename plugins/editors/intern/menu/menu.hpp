#pragma once

#include <windows.h>

#include <logger2.h>
#include <plugin2.h>

namespace flow::editors::menu {
void
init(HOST_APP_TABLE *host, LOG_HANDLE *logger);
}  // namespace flow::editors::menu
