#include "../property.hpp"

#include "effect.hpp"
#include "expression.hpp"
#include "value.hpp"

namespace flow::editor::menu::property {
void
init(HOST_APP_TABLE *host, LOG_HANDLE *logger, EDIT_HANDLE *context) {
    value::init(host, logger, context);
    expression::init(host, logger, context);
    effect::init(host, logger, context);
}
}  // namespace flow::editor::menu::property
