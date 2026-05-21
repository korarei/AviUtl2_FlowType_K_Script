#include "text.hpp"

#include <cmath>
#include <cstdint>
#include <format>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

#include <utf8.h>
#include <Eigen/Dense>

#include <font.hpp>
#include <utilities.hpp>

namespace {
namespace string = flow::string;
using HB_Font = flow::font::HB_Font;
using HB_Buffer = flow::font::HB_Buffer;
using FontCache = flow::font::FontCache;
using ID = flow::font::ID;

constinit LOG_HANDLE *logger = nullptr;

[[nodiscard]] inline constexpr std::string_view
get_value(std::string_view section, const std::string &key, std::string_view def = "") {
    const std::string target = "\n" + key + "=";
    size_t st = section.find(target);

    if (st == std::string_view::npos)
        return def;

    st += target.length();
    const size_t ed = section.find_first_of("\r\n", st);
    if (ed == std::string_view::npos)
        return section.substr(st);

    return section.substr(st, ed - st);
}

[[nodiscard]] inline constexpr double
get_front_value(std::string_view section, const std::string &key, double def = 0.0) {
    const auto v = get_value(section, key);
    if (v.empty())
        return def;

    double result;
    if (!string::to_number(v.substr(0uz, v.find(',')), result))
        return def;

    return result;
}

[[nodiscard]] constexpr std::string
unescape(std::string_view s) {
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

[[nodiscard]] constexpr std::string
delete_tags(std::string_view s) {
    std::string result;
    result.reserve(s.size());

    size_t pos = 0uz;

    while (true) {
        const auto st = s.find('<', pos);

        if (st == std::string_view::npos) {
            result.append(s.substr(pos));
            break;
        }

        result.append(s.substr(pos, st - pos));

        const size_t ed = s.find('>', st);
        if (ed == std::string_view::npos) {
            result.append(s.substr(st));
            break;
        }

        pos = ed + 1uz;
    }

    return result;
}

[[nodiscard]] constexpr std::string
shift_object_indices(std::string_view s, int offset = -1) {
    std::string result;
    result.reserve(s.size());

    const std::string_view target = "[Object.";
    size_t pos = 0uz;

    while (true) {
        const auto section = s.find(target, pos);

        if (section == std::string_view::npos) {
            result.append(s.substr(pos));
            break;
        }

        result.append(s.substr(pos, section - pos));

        const size_t st = section + target.length();
        const size_t ed = s.find(']', st);

        if (ed == std::string_view::npos) {
            result.append(s.substr(section));
            break;
        }

        int index;
        if (string::to_number(s.substr(st, ed - st), index)) {
            result.append("[Object.");
            result.append(std::to_string(index + offset));
            result.push_back(']');
        } else {
            result.append(s.substr(section, ed - section + 1uz));
        }

        pos = ed + 1uz;
    }

    return result;
}

constexpr std::vector<Eigen::Vector2d>
layout_text(const std::string &text, const std::string &name, double size, std::string_view align) {
    const auto direction = align.starts_with("縦書") ? HB_DIRECTION_TTB : HB_DIRECTION_LTR;

    std::vector<Eigen::Vector2d> result;
    result.reserve(text.size() / 2uz);

    hb_face_t *face = nullptr;

    if (!FontCache::load(face, ID::TextSplit, name))
        throw std::runtime_error("Font file not found");

    HB_Font font(hb_font_create(face));
    hb_font_set_scale(font.get(), static_cast<int>(size * 64.0), static_cast<int>(size * 64.0));

    HB_Buffer buffer(hb_buffer_create());

    hb_font_extents_t extents;
    hb_font_get_extents_for_direction(font.get(), direction, &extents);
    const double line_height = (extents.ascender - extents.descender + extents.line_gap) / 64.0;

    std::vector<std::vector<Eigen::Vector2d>> rows;
    Eigen::Vector2d line_origin(0.0, 0.0);
    std::istringstream iss(text + "\n");
    std::string line;

    while (std::getline(iss, line)) {
        if (line.empty()) {
            if (direction == HB_DIRECTION_LTR)
                line_origin.y() -= line_height;
            else
                line_origin.x() -= line_height;

            continue;
        }

        std::vector<Eigen::Vector2d> row;
        row.reserve(line.size() / 2uz);

        hb_buffer_reset(buffer.get());
        hb_buffer_add_utf8(buffer.get(), line.c_str(), -1, 0, -1);
        hb_buffer_set_direction(buffer.get(), direction);
        hb_buffer_guess_segment_properties(buffer.get());

        hb_shape(font.get(), buffer.get(), nullptr, 0);

        uint32_t count;
        const hb_glyph_info_t *info = hb_buffer_get_glyph_infos(buffer.get(), &count);

        Eigen::Vector2d nominal(0.0, 0.0);

        for (uint32_t i = 0u; i < count; ++i) {
            auto it = line.begin() + info[i].cluster;
            utf8::utfchar32_t cp = 0;
            try {
                cp = utf8::next(it, line.end());
            } catch (...) {
                throw std::runtime_error("Character encoding is unsupported in the text");
            }

            if ((cp <= 0x1f) || (cp == 0x7f) || (cp >= 0x80 && cp <= 0x9f))
                continue;

            row.emplace_back(line_origin + nominal);

            if (direction == HB_DIRECTION_LTR)
                nominal.x() += hb_font_get_glyph_h_advance(font.get(), info[i].codepoint) / 64.0;
            else
                nominal.y() += hb_font_get_glyph_v_advance(font.get(), info[i].codepoint) / 64.0;
        }

        if (!row.empty()) {
            const auto n = row.size() - 1uz;

            if (direction == HB_DIRECTION_LTR) {
                if (align.starts_with("中央揃え")) {
                    const auto origin = std::lerp(row[n].x(), nominal.x(), 0.5) - row[n].x() * 0.5;
                    for (size_t i = 0uz; i < n; ++i) row[i].x() = std::lerp(row[i].x(), row[i + 1uz].x(), 0.5) - origin;
                    row[n].x() = row[n].x() * 0.5;
                } else if (align.starts_with("右寄せ")) {
                    const auto origin = nominal.x();
                    for (size_t i = 0uz; i < n; ++i) row[i].x() = row[i + 1uz].x() - origin;
                    row[n].x() = 0.0;
                }
            } else {
                if (align.starts_with("縦書 中央")) {
                    const auto origin = std::lerp(row[n].y(), nominal.y(), 0.5) - row[n].y() * 0.5;
                    for (size_t i = 0uz; i < n; ++i) row[i].y() = std::lerp(row[i].y(), row[i + 1uz].y(), 0.5) - origin;
                    row[n].y() = row[n].y() * 0.5;
                } else if (align.starts_with("縦書 下寄")) {
                    const auto origin = nominal.y();
                    for (size_t i = 0uz; i < n; ++i) row[i].y() = row[i + 1uz].y() - origin;
                    row[n].y() = 0.0;
                }
            }
        }

        rows.push_back(std::move(row));

        if (direction == HB_DIRECTION_LTR)
            line_origin.y() -= line_height;
        else
            line_origin.x() -= line_height;
    }

    if (rows.empty())
        return {};

    Eigen::Vector2d origin(0.0, 0.0);

    if (direction == HB_DIRECTION_LTR) {
        if (align.ends_with("[中]"))
            origin.y() = (line_origin.y() + line_height) * 0.5;
        else if (align.ends_with("[下]"))
            origin.y() = line_origin.y() + line_height;
    } else {
        if (align.ends_with("[中]"))
            origin.x() = (line_origin.x() + line_height) * 0.5;
        else if (align.ends_with("[左]"))
            origin.x() = line_origin.x() + line_height;
    }

    for (const auto &row : rows) {
        for (const auto &p : row) {
            result.emplace_back(p - origin);
        }
    }

    return result;
}

constexpr void
split_text(EDIT_SECTION *edit) {
    constexpr std::string_view empty_object_alias =
            "{}"
            "[Object.0]\n"
            "effect.name=グループ制御\n"
            "X={}\n"
            "Y={}\n"
            "Z={}\n"
            "Group=1\n"
            "X軸回転={}\n"
            "Y軸回転={}\n"
            "Z軸回転={}\n"
            "拡大率={}\n"
            "対象レイヤー数={}\n"
            "{}";

    constexpr std::string_view text_object_alias =
            "{}"
            "[Object.0]\n"
            "effect.name=テキスト\n"
            "サイズ={}\n"
            "字間=0.00\n"
            "行間=0.00\n"
            "表示速度=0.00\n"
            "フォント={}\n"
            "文字色={}\n"
            "影・縁色={}\n"
            "文字装飾={}\n"
            "文字揃え={}\n"
            "B={}\n"
            "I={}\n"
            "テキスト={}\n"
            "文字毎に個別オブジェクト=0\n"
            "自動スクロール=0\n"
            "移動座標上に表示=0\n"
            "オブジェクトの長さを自動調節=0\n"
            "[Object.1]\n"
            "effect.name=標準描画\n"
            "X={}\n"
            "Y={}\n"
            "Z=0.00\n"
            "Group=1\n"
            "中心X=0.00\n"
            "中心Y=0.00\n"
            "中心Z=0.00\n"
            "X軸回転=0.00\n"
            "Y軸回転=0.00\n"
            "Z軸回転=0.00\n"
            "Group2=1\n"
            "拡大率=100.000\n"
            "縦横比={}\n"
            "透明度={}\n"
            "合成モード={}";

    const auto handle = edit->get_selected_object(0);
    const auto alias = string::as_string_view(edit->get_object_alias(handle));

    if (alias.empty())
        return;

    const auto p0 = alias.find("[Object.0]"), p1 = alias.find("[Object.1]"), p2 = alias.find("[Object.2]");
    if (p0 == std::string_view::npos || p1 == std::string_view::npos) {
        logger->error(logger, L"The alias does not match the expected format");
        return;
    }

    const auto obj = alias.substr(0uz, p0);
    const auto obj0 = alias.substr(p0, p1 - p0);
    const auto obj1 = alias.substr(p1, p2 - p1);
    const auto obj2 = p2 == std::string_view::npos ? "" : alias.substr(p2);
    if (obj0.find("effect.name=テキスト") == std::string_view::npos) {
        logger->error(logger, L"Text object is not selected");
        return;
    }

    auto text = std::string(get_value(obj0, "テキスト"));

    if (text.empty())
        return;

    text = unescape(text);
    text = delete_tags(text);

    if (text.empty())
        return;

    auto font = std::string(get_value(obj0, "フォント"));
    const auto size = get_front_value(obj0, "サイズ");
    if (font.empty() || size < 1.0e-6) {
        logger->error(logger, L"Font name is empty or size is zero");
        return;
    }

    const auto align = get_value(obj0, "文字揃え");

    std::vector<Eigen::Vector2d> coords;
    try {
        coords = layout_text(text, font, size, align);
    } catch (const std::exception &e) {
        logger->error(logger, string::to_wstring(string::as_utf8(e.what())).c_str());
        return;
    } catch (...) {
        logger->error(logger, L"Non-standard exception occurred");
        return;
    }

    if (coords.empty()) {
        logger->error(logger, L"Text layout computation failed");
        return;
    }

    const auto info = edit->get_object_layer_frame(handle);

    if (const auto h = edit->find_object(info.layer + 1, info.start);
        h != nullptr && edit->get_object_layer_frame(h).start <= info.end) {
        logger->error(logger, L"Target layer is occupied by another object");
        return;
    }

    std::vector<std::string> chars;
    auto it = text.begin();
    while (it != text.end()) {
        const auto st = it;
        utf8::utfchar32_t cp = 0;

        try {
            cp = utf8::next(it, text.end());
        } catch (...) {
            logger->error(logger, L"Character encoding is unsupported in the text");
            return;
        }

        if ((cp <= 0x1f) || (cp == 0x7f) || (cp >= 0x80 && cp <= 0x9f))
            continue;

        if (const auto h = edit->find_object(info.layer + static_cast<int>(chars.size()) + 2, info.start);
            h != nullptr && edit->get_object_layer_frame(h).start <= info.end) {
            logger->error(logger, L"Target layer is occupied by another object");
            return;
        }

        chars.emplace_back(st, it);
    }

    if (chars.size() == 1uz)
        return;

    if (chars.size() != coords.size()) {
        logger->error(logger, L"Character count mismatch between layout and text");
        return;
    }

    edit->delete_object(handle);

    edit->set_focus_object(edit->create_object_from_alias(
            std::format(
                    empty_object_alias,
                    obj,
                    get_value(obj1, "X", "0.00"),
                    get_value(obj1, "Y", "0.00"),
                    get_value(obj1, "Z", "0.00"),
                    get_value(obj1, "X軸回転", "0.00"),
                    get_value(obj1, "Y軸回転", "0.00"),
                    get_value(obj1, "Z軸回転", "0.00"),
                    get_value(obj1, "拡大率", "100.000"),
                    chars.size() + 1uz,
                    shift_object_indices(obj2))
                    .c_str(),
            info.layer,
            info.start,
            0));

    edit->create_object_from_alias(
            std::format(
                    empty_object_alias,
                    obj,
                    "-" + std::string(get_value(obj1, "中心X", "0.00")),
                    "-" + std::string(get_value(obj1, "中心Y", "0.00")),
                    "-" + std::string(get_value(obj1, "中心Z", "0.00")),
                    "0.00",
                    "0.00",
                    "0.00",
                    "100.000",
                    chars.size(),
                    "")
                    .c_str(),
            info.layer + 1,
            info.start,
            0);

    for (size_t i = 0uz; i < chars.size(); ++i) {
        edit->create_object_from_alias(
                std::format(
                        text_object_alias,
                        obj,
                        size,
                        font,
                        get_value(obj0, "文字色"),
                        get_value(obj0, "影・縁色"),
                        get_value(obj0, "文字装飾"),
                        align,
                        get_value(obj0, "B"),
                        get_value(obj0, "I"),
                        chars[i],
                        coords[i].x(),
                        -coords[i].y(),
                        get_value(obj1, "縦横比", "0.000"),
                        get_value(obj1, "透明度", "0.00"),
                        get_value(obj1, "合成モード", "通常"))
                        .c_str(),
                info.layer + static_cast<int>(i) + 2,
                info.start,
                0);
    }
}
}  // namespace

namespace flow::editors::menu::object::text {
void
init(HOST_APP_TABLE *host, LOG_HANDLE *handle) {
    logger = handle;

    host->register_object_menu(L"FlowType_K\\テキストを文字ごとに分解", split_text);
}
}  // namespace flow::editors::menu::object::text
