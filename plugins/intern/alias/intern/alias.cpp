#include "../alias.hpp"

#include <format>

namespace flow::alias {
Object::Object(const std::string &alias) : data(alias) {}

Object::Effect
Object::get(int index) const {
    const std::string_view sv(data);

    if (index < 0) {
        const size_t st = sv.find("[Object]");

        if (st == std::string_view::npos)
            return Effect("");

        return Effect(sv.substr(st, sv.find("[Object.0]") - st));
    }

    const size_t st = sv.find(std::format("[Object.{}]", index));
    if (st == std::string_view::npos)
        return Effect("");

    return Effect(sv.substr(st, sv.find(std::format("[Object.{}]", index + 1)) - st));
}

Object::Effect::Effect(std::string_view alias) : data(alias) {}

std::string
Object::Effect::unescape(std::string_view s) {
    std::string result;
    result.reserve(s.size());

    size_t pos = 0uz;

    while (true) {
        const auto curr = s.find('\\', pos);
        const auto next = curr + 1uz;

        if (curr == std::string_view::npos) {
            result.append(s.substr(pos));
            break;
        }

        result.append(s.substr(pos, curr - pos));
        if (next >= s.size()) {
            result.push_back('\\');
            break;
        }

        switch (s[next]) {
            case 'n':
                result.push_back('\n');
                break;
            case '\\':
                result.push_back('\\');
                break;
            default:
                result.push_back('\\');
                result.push_back(s[next]);
                break;
        }

        pos = next + 1uz;
    }

    return result;
}

std::string_view
Object::Effect::get(const std::string &key, std::string_view def) const {
    const std::string target = "\n" + key + "=";
    size_t st = data.find(target);

    if (st == std::string_view::npos)
        return def;

    st += target.length();
    return data.substr(st, data.find_first_of("\r\n", st) - st);
}

std::string_view
Object::Effect::front(const std::string &key, std::string_view def) const {
    const auto v = get(key);
    if (v.empty())
        return def;

    return v.substr(0uz, v.find(','));
}
}  // namespace flow::alias
