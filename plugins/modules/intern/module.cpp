#include "../module.hpp"

#include "hash/hash.hpp"
#include "island/island.hpp"
#include "text/kerning.hpp"
#include "text/regex.hpp"
#include "toml/toml.hpp"
#include "trim/trim.hpp"
#include "vector/vector.hpp"

namespace flow::module {
void
init(HOST_APP_TABLE *host, LOG_HANDLE *logger) {
    hash::init(host, logger);
    island::init(host, logger);
    text::kerning::init(host, logger);
    text::regex::init(host, logger);
    toml::init(host, logger);
    trim::init(host, logger);
    vector::init(host, logger);
}

void
deinit() {
    text::kerning::deinit();
    text::regex::deinit();
}
}  // namespace flow::module
