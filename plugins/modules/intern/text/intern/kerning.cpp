#include "../kerning.hpp"

#include <cstdint>
#include <memory>
#include <sstream>
#include <string>

#include <utf8.h>
#include <Eigen/Dense>

#include <module2.h>

#include <intern/font/font.hpp>
#include <intern/utilities.hpp>

namespace {
namespace string = flow::string;
using HB_Font = flow::font::HB_Font;
using HB_Buffer = flow::font::HB_Buffer;
using FontCache = flow::font::FontCache;
using Cache = flow::cache::Cache<HB_Font, std::string>;

constinit LOG_HANDLE *logger = nullptr;

Cache fonts;
thread_local HB_Buffer buffer;

constexpr void
shift(SCRIPT_MODULE_PARAM *param) {
    constexpr hb_feature_t features[] = {
            {HB_TAG('l', 'i', 'g', 'a'), 0, HB_FEATURE_GLOBAL_START, HB_FEATURE_GLOBAL_END},
            {HB_TAG('c', 'l', 'i', 'g'), 0, HB_FEATURE_GLOBAL_START, HB_FEATURE_GLOBAL_END},
            {HB_TAG('d', 'l', 'i', 'g'), 0, HB_FEATURE_GLOBAL_START, HB_FEATURE_GLOBAL_END},
            {HB_TAG('p', 'a', 'l', 't'), 1, HB_FEATURE_GLOBAL_START, HB_FEATURE_GLOBAL_END},
            {HB_TAG('v', 'p', 'a', 'l'), 1, HB_FEATURE_GLOBAL_START, HB_FEATURE_GLOBAL_END},
            {HB_TAG('k', 'e', 'r', 'n'), 1, HB_FEATURE_GLOBAL_START, HB_FEATURE_GLOBAL_END},
            {HB_TAG('v', 'k', 'r', 'n'), 1, HB_FEATURE_GLOBAL_START, HB_FEATURE_GLOBAL_END},
    };

    const int n = param->get_param_num();
    if (n != 4 && n != 5) {
        param->set_error("Function call has wrong argument count");
        return;
    }

    const int64_t id = static_cast<int64_t>(param->get_param_double(0));
    const std::string text = string::as_string(param->get_param_string(1));
    const double size = param->get_param_double(2);
    const std::string name = string::as_string(param->get_param_string(3));
    const std::string_view align = n > 4 ? string::as_string_view(param->get_param_string(4)) : "";

    if (text.empty())
        return;

    if (name.empty() || size < 1.0e-6) {
        param->set_error("Font name is empty or size is zero");
        return;
    }

    const auto direction = align.starts_with("縦書") ? HB_DIRECTION_TTB : HB_DIRECTION_LTR;

    hb_face_t *face = nullptr;
    try {
        if (!FontCache::load(face, id, name)) {
            logger->warn(logger, L"Font file not found and using fallback");

            auto it = text.begin();
            while (it != text.end()) {
                utf8::utfchar32_t cp = 0;

                try {
                    cp = utf8::next(it, text.end());
                } catch (...) {
                    param->set_error("Character encoding is unsupported in the text");
                    return;
                }

                if ((cp <= 0x1f) || (cp == 0x7f) || (cp >= 0x80 && cp <= 0x9f))
                    continue;

                double result[] = {0.0, 0.0};

                param->push_result_array_double(result, 2);
            }
            return;
        }
    } catch (const std::exception &e) {
        param->set_error(e.what());
        return;
    }

    {
        auto entry = fonts.fetch(id, name);

        if (*entry == nullptr)
            *entry = HB_Font(hb_font_create(face));

        auto *font = entry->get();
        hb_font_set_scale(font, static_cast<int>(size * 64.0), static_cast<int>(size * 64.0));

        if (buffer == nullptr)
            buffer = HB_Buffer(hb_buffer_create());

        std::istringstream iss(text);
        std::string line;

        while (std::getline(iss, line)) {
            if (line.empty())
                continue;

            hb_buffer_reset(buffer.get());
            hb_buffer_add_utf8(buffer.get(), line.c_str(), -1, 0, -1);
            hb_buffer_set_direction(buffer.get(), direction);
            hb_buffer_guess_segment_properties(buffer.get());

            hb_shape(font, buffer.get(), features, std::size(features));

            uint32_t count;
            const hb_glyph_info_t *info = hb_buffer_get_glyph_infos(buffer.get(), &count);
            const hb_glyph_position_t *pos = hb_buffer_get_glyph_positions(buffer.get(), nullptr);

            Eigen::Vector2d nominal(0.0, 0.0);
            Eigen::Vector2d actual(0.0, 0.0);

            for (uint32_t i = 0u; i < count; ++i) {
                auto it = line.begin() + info[i].cluster;
                utf8::utfchar32_t cp = 0;
                try {
                    cp = utf8::next(it, line.end());
                } catch (...) {
                    param->set_error("Character encoding is unsupported in the text");
                    return;
                }

                if ((cp <= 0x1f) || (cp == 0x7f) || (cp >= 0x80 && cp <= 0x9f))
                    continue;

                Eigen::Vector2<hb_position_t> origin(0, 0);
                hb_font_add_glyph_origin_for_direction(font, info[i].codepoint, direction, &origin.x(), &origin.y());
                const Eigen::Vector2d offset(
                        (pos[i].x_offset + origin.x()) / 64.0, -(pos[i].y_offset + origin.y()) / 64.0);

                Eigen::Vector2d result = actual - nominal + offset;
                param->push_result_array_double(result.data(), 2);

                if (direction == HB_DIRECTION_LTR)
                    nominal.x() += hb_font_get_glyph_h_advance(font, info[i].codepoint) / 64.0;
                else
                    nominal.y() -= hb_font_get_glyph_v_advance(font, info[i].codepoint) / 64.0;

                actual.x() += pos[i].x_advance / 64.0;
                actual.y() -= pos[i].y_advance / 64.0;
            }
        }
    }
}

constinit SCRIPT_MODULE_FUNCTION functions[] = {
        {L"shift", shift},
        {nullptr, nullptr},
};

constinit SCRIPT_MODULE_TABLE info = {
        .information = L"Kerning@FlowType_K v" VERSION L" by Korarei",
        .functions = functions,
};
}  // namespace

namespace flow::modules::text::kerning {
void
init(HOST_APP_TABLE *host, LOG_HANDLE *handle) {
    logger = handle;

    host->register_script_module_name(&info, L"Kerning@FlowType_K");
    host->register_clear_cache_handler([]([[maybe_unused]] EDIT_SECTION *edit) { fonts.reset(); });
}

void
deinit() {
    fonts.reset();
}
}  // namespace flow::modules::text::kerning
