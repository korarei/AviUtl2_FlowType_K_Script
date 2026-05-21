#include "editor.hpp"

#include <menu.hpp>

namespace {
constinit LOG_HANDLE *logger = nullptr;
}  // namespace

namespace flow::editors {
void
init(HOST_APP_TABLE *host, LOG_HANDLE *handle) {
    logger = handle;

    menu::init(host, logger);
}
}  // namespace flow::editors
