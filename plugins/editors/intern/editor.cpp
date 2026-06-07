#include "../editor.hpp"

#include "menu/menu.hpp"

namespace flow::editor {
void
init(HOST_APP_TABLE *host, LOG_HANDLE *logger) {
    auto editor = host->create_edit_handle();

    menu::init(host, logger, editor);
}
}  // namespace flow::editor
