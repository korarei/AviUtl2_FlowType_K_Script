#include "object.hpp"

#include "text.hpp"

namespace flow::editors::menu::object {
void
init(HOST_APP_TABLE *host, LOG_HANDLE *logger) {
    flow::editors::menu::object::text::init(host, logger);
}
}  // namespace flow::editors::menu::object
