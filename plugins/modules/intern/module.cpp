#include "../module.hpp"

#include "hash/hash.hpp"
#include "island/island.hpp"
#include "text/kerning.hpp"
#include "text/regex.hpp"
#include "text/utf8.hpp"
#include "toml/toml.hpp"
#include "vector/vector.hpp"

namespace flow::module {
void
init(HOST_APP_TABLE *host, LOG_HANDLE *logger) {
    hash::init(host, logger);
    island::init(host, logger);
    text::kerning::init(host, logger);
    text::regex::init(host, logger);
    text::utf8::init(host, logger);
    toml::init(host, logger);
    vector::init(host, logger);
}

void
deinit() {
    island::deinit();
}
}  // namespace flow::module
