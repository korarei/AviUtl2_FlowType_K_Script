#include "module.hpp"

#include <hash.hpp>
#include <island.hpp>
#include <kerning.hpp>
#include <regex.hpp>
#include <toml.hpp>
#include <trim.hpp>
#include <vector.hpp>

namespace flow::modules {
void
init(HOST_APP_TABLE *host, LOG_HANDLE *logger) {
    hash::init(host, logger);
    vector::init(host, logger);
    trim::init(host, logger);
    island::init(host, logger);
    toml::init(host, logger);
    text::regex::init(host, logger);
    text::kerning::init(host, logger);
}

void
deinit() {
    text::regex::deinit();
    text::kerning::deinit();
}
}  // namespace flow::modules
