#pragma once

#include <windows.h>

#include <logger2.h>
#include <plugin2.h>

namespace flow::editor::menu::property::expression {
void
init(HOST_APP_TABLE *host, LOG_HANDLE *logger, EDIT_HANDLE *editor);
}  // namespace flow::editor::menu::property::expression
