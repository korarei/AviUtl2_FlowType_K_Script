#include "../island.hpp"

#include <chrono>
#include <execution>
#include <format>
#include <unordered_map>
#include <vector>

#include <Eigen/Dense>

#include <module2.h>

namespace {
struct RGBA {
    uint8_t r, g, b, a;
};

struct Image {
    Eigen::Vector2i size;
    RGBA *data;
};

struct Island {
    Eigen::Vector2i origin, size;
    Eigen::Vector2d delta;
    Eigen::Vector2i key;
    int label;
};

struct Run {
    int x0, x1, label;
};

class UnionFind {
public:
    UnionFind(int n = 0) : data(n, -1), count(n) {}

    [[nodiscard]] constexpr int find(int x) {
        if (data[x] < 0)
            return x;

        return data[x] = find(data[x]);
    }

    constexpr void unite(int a, int b) {
        a = find(a);
        b = find(b);
        if (a == b)
            return;

        if (data[a] > data[b])
            std::swap(a, b);

        data[a] += data[b];
        data[b] = a;
        count--;
    }

    constexpr void ensure(int x) {
        const int old_size = static_cast<int>(data.size());
        if (x < old_size)
            return;

        const int new_size = std::max(x + 1, old_size * 2);
        data.resize(new_size, -1);
        count += new_size - old_size;
    }

private:
    std::vector<int> data;
    int count;
};

constinit LOG_HANDLE *logger = nullptr;
thread_local std::unordered_map<int64_t, std::vector<Island>> islands;

constexpr void
scan(SCRIPT_MODULE_PARAM *param) {
    if (param->get_param_num() != 11) {
        param->set_error("Function call has wrong argument count");
        return;
    }

    const Image img{
            .size = Eigen::Vector2i(param->get_param_int(9), param->get_param_int(10)),
            .data = static_cast<RGBA *>(param->get_param_data(8)),
    };

    if (img.size.x() <= 0 || img.size.y() <= 0 || img.data == nullptr) {
        param->set_error("Pixel data is null or corrupted");
        return;
    }

    const int64_t id = static_cast<int64_t>(param->get_param_double(0));
    const int threshold = param->get_param_int(1);
    const int connectivity = param->get_param_int(2);
    const int primary_axis = param->get_param_int(3);
    const Eigen::Vector2<bool> order(param->get_param_int(4) != 1, param->get_param_int(5) != 1);
    const Eigen::Vector2d blocks(param->get_param_double(6), param->get_param_double(7));
    const Eigen::Vector2d texel = (blocks.array() > 1.0).select(blocks.array() / img.size.cast<double>().array(), 1.0);

    using Clock = std::chrono::steady_clock;
    const auto t0 = Clock::now();

    std::vector<std::vector<Run>> runs(img.size.y());
    std::for_each(std::execution::par, runs.begin(), runs.end(), [&](auto &row) {
        const int y = static_cast<int>(&row - &runs[0]);
        const RGBA *p = img.data + y * img.size.x();

        int x = 0;
        while (x < img.size.x()) {
            if (p[x].a <= threshold) {
                ++x;
                continue;
            }

            int x0 = x;
            while (x < img.size.x() && p[x].a > threshold) ++x;

            row.emplace_back(x0, x - 1, 0);
        }
    });

    const auto t1 = Clock::now();

    UnionFind uf(img.size.y() * 16 + 256);
    int label = 1;

    for (auto &r : runs[0]) {
        uf.ensure(label);
        r.label = label++;
    }

    for (int y = 1; y < img.size.y(); ++y) {
        const auto &prev = runs[y - 1];
        const int size = static_cast<int>(prev.size());

        auto &curr = runs[y];
        int i = 0;
        for (auto &r : curr) {
            while (i < size && prev[i].x1 < r.x0 - connectivity) ++i;

            int j = i;
            while (j < size && prev[j].x0 <= r.x1 + connectivity) {
                const auto &p = prev[j];
                if (r.label == 0)
                    r.label = p.label;
                else
                    uf.unite(r.label, p.label);

                ++j;
            }

            if (r.label == 0) {
                uf.ensure(label);
                r.label = label++;
            }
        }
    }

    const auto t2 = Clock::now();

    std::vector<Eigen::AlignedBox2i> boxes(label);
    for (int y = 0; y < img.size.y(); ++y) {
        for (auto &r : runs[y]) {
            r.label = uf.find(r.label);
            boxes[r.label].extend(Eigen::Vector2i(r.x0, y));
            boxes[r.label].extend(Eigen::Vector2i(r.x1, y));
        }
    }

    std::vector<Island> tmp;
    tmp.reserve(label);
    for (int i = 1; i < label; ++i) {
        const auto &b = boxes[i];
        if (b.isEmpty())
            continue;

        const Eigen::Vector2d center = b.cast<double>().center();
        tmp.emplace_back(
                b.min(),
                b.sizes().array() + 1,
                center.array() - img.size.cast<double>().array() * 0.5 + 0.5,
                center.cwiseProduct(texel).array().floor().cast<int>(),
                i);
    }

    if (tmp.size() > 262144uz) {
        param->set_error("Too many islands detected and exceeds maximum limit");
        return;
    }

    const auto t3 = Clock::now();

    const int s1 = primary_axis, s2 = 1 - primary_axis;
    std::sort(std::execution::par_unseq, tmp.begin(), tmp.end(), [&](const Island &a, const Island &b) {
        if (a.key(s1) != b.key(s1))
            return (a.key(s1) < b.key(s1)) == order(s1);

        return (a.key(s2) < b.key(s2)) == order(s2);
    });

    auto &isls = islands[id];
    isls = std::move(tmp);
    std::vector<int> to_idx(label, -1);
    for (size_t i = 0; i < isls.size(); ++i) to_idx[isls[i].label] = static_cast<int>(i);

    const auto t4 = Clock::now();

    std::for_each(std::execution::par_unseq, runs.begin(), runs.end(), [&](const auto &row) {
        const int y = static_cast<int>(&row - &runs[0]);
        RGBA *line = img.data + y * img.size.x();
        int prev = 0;

        for (const auto &r : row) {
            if (r.x0 > prev)
                std::memset(line + prev, 0, sizeof(RGBA) * (r.x0 - prev));

            const int idx = to_idx[r.label];
            if (idx >= 0)
                std::fill_n(
                        line + r.x0,
                        r.x1 - r.x0 + 1,
                        std::bit_cast<RGBA>(static_cast<uint32_t>(idx) * 4u | 0xFF000000u));
            else
                std::memset(line + r.x0, 0, sizeof(RGBA) * (r.x1 - r.x0 + 1));

            prev = r.x1 + 1;
        }

        if (prev < img.size.x())
            std::memset(line + prev, 0, sizeof(RGBA) * (img.size.x() - prev));
    });

    const auto t5 = Clock::now();

    auto ms = [](auto d) { return std::chrono::duration_cast<std::chrono::microseconds>(d).count() * 0.001; };
    auto msg = std::format(
            L"[island::scan] {}x{} n={} | runs={:.3f} uf={:.3f} bbox={:.3f} sort={:.3f} write={:.3f} total={:.3f} ms",
            img.size.x(),
            img.size.y(),
            islands.size(),
            ms(t1 - t0),
            ms(t2 - t1),
            ms(t3 - t2),
            ms(t4 - t3),
            ms(t5 - t4),
            ms(t5 - t0));

    logger->verbose(logger, msg.c_str());

    if (isls.empty())
        logger->warn(logger, L"No islands detected at this threshold");

    param->push_result_int(static_cast<int>(isls.size()));
}

constexpr void
fetch(SCRIPT_MODULE_PARAM *param) {
    const int64_t id = static_cast<int64_t>(param->get_param_double(0));
    const size_t idx = static_cast<size_t>(param->get_param_double(1));

    const auto it = islands.find(id);

    if (it == islands.end()) {
        param->set_error("Cache is empty and scan must be performed first");
        return;
    }

    const auto &isls = it->second;

    if (idx >= isls.size()) {
        param->set_error("Selected island index is out of bounds");
        return;
    }

    const auto &isl = isls[idx];

    param->push_result_int(isl.origin.x());
    param->push_result_int(isl.origin.y());
    param->push_result_int(isl.size.x());
    param->push_result_int(isl.size.y());
    param->push_result_double(isl.delta.x());
    param->push_result_double(isl.delta.y());
}

constexpr void
reset(SCRIPT_MODULE_PARAM *param) {
    islands.erase(static_cast<int64_t>(param->get_param_double(0)));
}

constinit SCRIPT_MODULE_FUNCTION functions[] = {
        {L"scan", scan},
        {L"fetch", fetch},
        {L"reset", reset},
        {nullptr, nullptr},
};

constinit SCRIPT_MODULE_TABLE info = {
        .information = L"Island@FlowType_K v" VERSION L" by Korarei",
        .functions = functions,
};
}  // namespace

namespace flow::module::island {
void
init(HOST_APP_TABLE *host, LOG_HANDLE *handle) {
    logger = handle;

    host->register_script_module_name(&info, L"Island@FlowType_K");
}
}  // namespace flow::module::island
