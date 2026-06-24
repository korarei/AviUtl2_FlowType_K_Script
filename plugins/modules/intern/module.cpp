#include "../module.hpp"

#include "hash/hash.hpp"
#include "island/island.hpp"
#include "text/text.hpp"
#include "toml/toml.hpp"
#include "vector/vector.hpp"

namespace flow::module {
void
init(HOST_APP_TABLE *host, LOG_HANDLE *logger) {
    hash::init(host, logger);
    island::init(host, logger);
    // text::init(host, logger);
    toml::init(host, logger);
    vector::init(host, logger);
}

void
deinit() {
    island::deinit();
}
}  // namespace flow::module
