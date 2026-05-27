#pragma once

#include <string>
#include <string_view>

namespace flow::alias {
class Object {
public:
    class Effect {
    public:
        explicit Effect(std::string_view alias);

        [[nodiscard]] std::string_view get(const std::string &key, std::string_view def = "") const;
        [[nodiscard]] std::string_view front(const std::string &key, std::string_view def = "") const;
        [[nodiscard]] std::string_view alias() const noexcept { return data; }

    private:
        std::string_view data;
    };

    explicit Object(const std::string &alias);

    [[nodiscard]] Effect get(int index) const;
    [[nodiscard]] std::string_view alias() const noexcept { return data; }

private:
    std::string data;
};
}  // namespace flow::alias
