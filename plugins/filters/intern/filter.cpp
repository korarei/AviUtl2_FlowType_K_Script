#include "../filter.hpp"

#include "align/align.hpp"
#include "deform/deform.hpp"
#include "transform/transform.hpp"

namespace flow::filter {
void
init(HOST_APP_TABLE *host, LOG_HANDLE *logger) {
    align::init(host, logger);
    deform::init(host, logger);
    transform::init(host, logger);
}
}  // namespace flow::filter
