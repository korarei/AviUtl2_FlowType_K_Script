#include "../menu.hpp"

#include "object/object.hpp"
#include "property/property.hpp"

namespace flow::editor::menu {
void
init(HOST_APP_TABLE *host, LOG_HANDLE *logger) {
    object::init(host, logger);
    property::init(host, logger);
}
}  // namespace flow::editor::menu
