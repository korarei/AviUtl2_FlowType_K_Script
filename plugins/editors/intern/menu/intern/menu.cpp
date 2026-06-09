#include "../menu.hpp"

#include "object/object.hpp"
#include "property/property.hpp"

namespace flow::editor::menu {
void
init(HOST_APP_TABLE *host, LOG_HANDLE *logger, EDIT_HANDLE *context) {
    object::init(host, logger);
    property::init(host, logger, context);
}
}  // namespace flow::editor::menu
