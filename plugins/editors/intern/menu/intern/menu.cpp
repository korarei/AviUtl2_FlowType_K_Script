#include "menu.hpp"

#include <object.hpp>
#include <property.hpp>

namespace flow::editors::menu {
void
init(HOST_APP_TABLE *host, LOG_HANDLE *logger) {
    object::init(host, logger);
    property::init(host, logger);
}
}  // namespace flow::editors::menu
