#include "../property.hpp"

#include "effect.hpp"
#include "expression.hpp"
#include "value.hpp"

namespace flow::editor::menu::property {
void
init(HOST_APP_TABLE *host, LOG_HANDLE *logger, EDIT_HANDLE *editor) {
    value::init(host, logger, editor);
    effect::init(host, logger, editor);
    expression::init(host, logger, editor);
}
}  // namespace flow::editor::menu::property
