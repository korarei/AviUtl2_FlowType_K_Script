#include "../menu.hpp"

#include "object/object.hpp"
#include "property/property.hpp"

namespace flow::editor::menu {
void
init(HOST_APP_TABLE *host, LOG_HANDLE *logger, EDIT_HANDLE *editor) {
    object::init(host, logger);
    property::init(host, logger, editor);
}
}  // namespace flow::editor::menu
