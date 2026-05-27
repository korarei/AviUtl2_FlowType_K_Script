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
