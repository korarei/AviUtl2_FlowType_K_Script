#include "../text.hpp"

#include "kerning.hpp"
#include "property.hpp"
#include "regex.hpp"
#include "utf8.hpp"

namespace flow::module::text {
void
init(HOST_APP_TABLE *host, LOG_HANDLE *logger) {
    kerning::init(host, logger);
    property::init(host, logger);
    regex::init(host, logger);
    utf8::init(host, logger);
}
}  // namespace flow::module::text
