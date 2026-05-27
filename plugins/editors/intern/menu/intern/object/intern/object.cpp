#include "../object.hpp"

#include "text.hpp"

namespace flow::editor::menu::object {
void
init(HOST_APP_TABLE *host, LOG_HANDLE *logger) {
    flow::editor::menu::object::text::init(host, logger);
}
}  // namespace flow::editor::menu::object
