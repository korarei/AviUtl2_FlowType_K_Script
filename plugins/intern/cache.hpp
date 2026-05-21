#pragma once

#include <cstdint>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <string>
#include <unordered_map>

namespace flow::cache {
template <typename T, typename Name = std::wstring>
class Cache {
public:
    struct Entry {
        std::mutex mtx;
        Name name;
        T cache;

        constexpr explicit Entry(const Name &n) : name(n) {}
    };

    class LockedEntry {
    public:
        std::unique_lock<std::mutex> lock;

        explicit LockedEntry(std::shared_ptr<Entry> e) : entry(std::move(e)), lock(entry->mtx) {}

        T *operator->() const noexcept { return &entry->cache; }
        T &operator*() const noexcept { return entry->cache; }

    private:
        std::shared_ptr<Entry> entry;
    };

    [[nodiscard]] LockedEntry fetch(int64_t id, const Name &name) {
        {
            std::shared_lock lock(mtx);
            if (auto it = id_to_cache.find(id); it != id_to_cache.end()) {
                if (auto entry = it->second; entry && entry->name == name)
                    return LockedEntry{std::move(entry)};
            }
        }

        std::unique_lock lock(mtx);
        if (auto it = id_to_cache.find(id); it != id_to_cache.end()) {
            if (auto entry = it->second; entry && entry->name == name)
                return LockedEntry{std::move(entry)};

            release(id);
        }

        std::shared_ptr<Entry> entry;
        if (auto it = name_to_cache.find(name); it != name_to_cache.end()) {
            entry = it->second.lock();
            if (!entry)
                name_to_cache.erase(it);
        }

        if (!entry) {
            entry = std::make_shared<Entry>(name);
            name_to_cache[name] = entry;
        }

        return LockedEntry{id_to_cache[id] = std::move(entry)};
    }

    constexpr void reset(int64_t id) {
        std::unique_lock lock(mtx);
        release(id);
    }

    constexpr void reset(const Name &name) {
        std::unique_lock lock(mtx);
        release(name);
    }

    constexpr void reset() {
        std::unique_lock lock(mtx);
        std::unordered_map<Name, std::weak_ptr<Entry>>{}.swap(name_to_cache);
        std::unordered_map<int64_t, std::shared_ptr<Entry>>{}.swap(id_to_cache);
    }

private:
    constexpr void release(int64_t id) {
        if (auto node = id_to_cache.extract(id)) {
            const auto &entry = node.mapped();
            if (entry) {
                if (auto it = name_to_cache.find(entry->name); it != name_to_cache.end() && it->second.expired())
                    name_to_cache.erase(it);
            }
        }
    }

    constexpr void release(const Name &name) {
        if (auto node = name_to_cache.extract(name))
            std::erase_if(id_to_cache, [&](const auto &e) { return e.second && e.second->name == name; });
    }

    std::shared_mutex mtx;
    std::unordered_map<Name, std::weak_ptr<Entry>> name_to_cache;
    std::unordered_map<int64_t, std::shared_ptr<Entry>> id_to_cache;
};
}  // namespace flow::cache
