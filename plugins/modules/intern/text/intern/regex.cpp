#include "../regex.hpp"

#include <cstdint>
#include <string_view>
#include <vector>

#include <re2/re2.h>
#include <utf8.h>

#include <module2.h>

#include <intern/cache.hpp>
#include <intern/regex/regex.hpp>
#include <intern/string.hpp>

#ifndef VERSION
#define VERSION L"0.1.0"
#endif

namespace {
namespace string = flow::string;
using RegexCache = flow::regex::RegexCache;

constinit LOG_HANDLE *logger = nullptr;

constexpr void
mark(SCRIPT_MODULE_PARAM *param) {
    const int n = param->get_param_num();
    if (n != 3 && n != 4) {
        param->set_error("Function call has wrong argument count");
        return;
    }

    const int64_t id = static_cast<int64_t>(param->get_param_double(0));
    const std::string_view input = string::as_string_view(param->get_param_string(1));
    if (input.empty())
        return;

    auto fallback = [&]() {
        auto it = input.begin();
        while (it != input.end()) {
            utf8::utfchar32_t cp = 0;

            try {
                cp = utf8::next(it, input.end());
            } catch (...) {
                param->set_error("Character encoding is unsupported in the text");
                return;
            }

            bool result[] = {true, (cp <= 0x1f) || (cp == 0x7f) || (cp >= 0x80 && cp <= 0x9f)};

            param->push_result_array_boolean(result, 2);
        }
    };

    const std::string pattern = string::as_string(param->get_param_string(2));
    if (pattern.empty()) {
        fallback();
        return;
    }

    int idx = param->get_param_int(3);

    std::vector<uint8_t> hits(input.size(), 0u);

    const auto re = RegexCache::compile(id, pattern);

    if (!re->ok()) {
        logger->warn(logger, L"Regex pattern syntax is incorrect");
        fallback();
        return;
    }

    const int nsubmatch = re->NumberOfCapturingGroups() + 1;

    re2::StringPiece text(input);
    std::vector<re2::StringPiece> match(nsubmatch);
    idx = idx >= 0 && idx < nsubmatch ? idx : 0;
    size_t pos = 0uz;

    while (pos <= input.size() && re->Match(text, pos, text.size(), re2::RE2::UNANCHORED, match.data(), nsubmatch)) {
        re2::StringPiece &m = match[idx];
        if (m.empty())
            m = match[0];

        const size_t st = m.data() - input.data();
        const size_t ed = st + m.size();

        for (size_t i = st; i < ed; ++i) hits[i] = 1u;

        if (m.size() > 0uz) {
            pos = ed;
            continue;
        }

        auto it = input.begin() + st;
        if (it == input.end())
            break;

        try {
            utf8::next(it, input.end());
        } catch (...) {
            param->set_error("Character encoding is unsupported in the text");
            return;
        }

        pos = it - input.begin();
    }

    auto it = input.begin();
    while (it != input.end()) {
        const size_t st = it - input.begin();
        utf8::utfchar32_t cp = 0;

        try {
            cp = utf8::next(it, input.end());
        } catch (...) {
            param->set_error("Character encoding is unsupported in the text");
            return;
        }

        const size_t ed = it - input.begin();

        bool result[] = {
                std::memchr(hits.data() + st, 1, ed - st) != nullptr,
                (cp <= 0x1f) || (cp == 0x7f) || (cp >= 0x80 && cp <= 0x9f),
        };

        param->push_result_array_boolean(result, 2);
    }
}

constinit SCRIPT_MODULE_FUNCTION functions[] = {
        {L"mark", mark},
        {nullptr, nullptr},
};

constinit SCRIPT_MODULE_TABLE info = {
        .information = L"Regex@FlowType_K v" VERSION L" by Korarei",
        .functions = functions,
};
}  // namespace

namespace flow::module::text::regex {
void
init(HOST_APP_TABLE *host, LOG_HANDLE *handle) {
    logger = handle;

    host->register_script_module_name(&info, L"Regex@FlowType_K");
}
}  // namespace flow::module::text::regex
