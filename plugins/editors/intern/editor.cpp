#include "../editor.hpp"

#include "menu/menu.hpp"

namespace flow::editor {
void
init(HOST_APP_TABLE *host, LOG_HANDLE *logger) {
    auto context = host->create_edit_handle();

    menu::init(host, logger, context);
}
}  // namespace flow::editor
