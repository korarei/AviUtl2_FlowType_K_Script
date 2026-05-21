#pragma once

#include <windows.h>

#include <logger2.h>
#include <plugin2.h>

namespace flow::modules::text::regex {
void
init(HOST_APP_TABLE *host, LOG_HANDLE *logger);

void
deinit();
}  // namespace flow::modules::text::regex
