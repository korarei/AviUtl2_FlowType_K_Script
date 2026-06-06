#include "../editor.hpp"

#include "menu/menu.hpp"

namespace {
constinit LOG_HANDLE *logger = nullptr;
constinit EDIT_HANDLE *editor = nullptr;
}  // namespace

namespace flow::editor {
void
init(HOST_APP_TABLE *host, LOG_HANDLE *handle) {
    logger = handle;
    ::editor = host->create_edit_handle();

    menu::init(host, logger, ::editor);
}
}  // namespace flow::editor
