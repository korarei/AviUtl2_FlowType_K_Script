#include "../regex.hpp"

namespace flow::regex {
RegexCache &
RegexCache::instance() {
    static RegexCache inst;
    return inst;
}

void
RegexCache::reset() {
    instance().cache::Cache<std::shared_ptr<re2::RE2>, std::string>::reset();
}

[[nodiscard]] std::shared_ptr<const re2::RE2>
RegexCache::compile(int64_t id, const std::string &pattern) {
    auto &self = instance();
    auto entry = self.fetch(id, pattern);

    if (*entry == nullptr)
        *entry = std::make_shared<re2::RE2>(pattern);

    return *entry;
}
}  // namespace flow::regex
