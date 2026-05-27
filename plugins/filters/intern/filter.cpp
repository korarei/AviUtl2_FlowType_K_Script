#include "../filter.hpp"

#include "deform/deform.hpp"
// #include "transform/transform.hpp"

namespace flow::filter {
void
init(HOST_APP_TABLE *host, LOG_HANDLE *logger) {
    deform::init(host, logger);
    // transform::init(host, logger);
}
}  // namespace flow::filter
