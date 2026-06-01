#pragma once

#include <memory>
#include <string>

#include <re2/re2.h>

#include <intern/cache.hpp>

namespace flow::regex {
class RegexCache : private cache::Cache<std::shared_ptr<re2::RE2>, std::string> {
public:
    [[nodiscard]] static std::shared_ptr<const re2::RE2> compile(int64_t id, const std::string &pattern);
    static void reset();

private:
    ~RegexCache() { reset(); }
    RegexCache() = default;

    RegexCache(const RegexCache &) = delete;
    RegexCache &operator=(const RegexCache &) = delete;
    RegexCache(RegexCache &&) = delete;
    RegexCache &operator=(RegexCache &&) = delete;

    [[nodiscard]] static RegexCache &instance();
};
}  // namespace flow::regex
