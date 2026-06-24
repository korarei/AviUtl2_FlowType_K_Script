--@Kerning

--require:${PROJECT_REQUIRES_AVIUTL2}
--information:Kerning@${SCRIPT_NAME} v${PROJECT_VERSION} by ${PROJECT_AUTHOR}
--label:${LABEL}

local kerning_mode = 1 --select@kerning_mode:Kerning Mode=1,None=0,Metrics=1
--group:Filter,true
local filter_regex_pattern = "" --string@filter_regex_pattern:Filter::Regex Pattern,
local filter_capture_group = 0 --track@filter_capture_group:Filter::Capture Group,0,20,0,1
local should_limit_fx = false --checksection@should_limit_fx:Filter::Limit Effects Below,false
--group:Transform,false
--separator:Pivot Point
local xform_pivot_x = 0.0 --track@xform_pivot_x:Transform::Pivot::X,-100000,100000,0,0.01
local xform_pivot_y = 0.0 --track@xform_pivot_y:Transform::Pivot::Y,-100000,100000,0,0.01
local xform_pivot_z = 0.0 --track@xform_pivot_z:Transform::Pivot::Z,-100000,100000,0,0.01
--trackgroup@xform_pivot_x,xform_pivot_y,xform_pivot_z:Group::Transform::Pivot
--separator:Position
local xform_position_x = 0.0 --track@xform_position_x:Transform::Position::X,-100000,100000,0,0.01
local xform_position_y = 0.0 --track@xform_position_y:Transform::Position::Y,-100000,100000,0,0.01
local xform_position_z = 0.0 --track@xform_position_z:Transform::Position::Z,-100000,100000,0,0.01
--trackgroup@xform_position_x,xform_position_y,xform_position_z:Group::Transform::Position
--separator:Rotation
local xform_rotation_w = 0.0 --track@xform_rotation_w:Transform::Rotation::W,-3600,3600,0,0.01
local xform_rotation_x = 0.0 --track@xform_rotation_x:Transform::Rotation::X,-3600,3600,0,0.01
local xform_rotation_y = 0.0 --track@xform_rotation_y:Transform::Rotation::Y,-3600,3600,0,0.01
local xform_rotation_z = 0.0 --track@xform_rotation_z:Transform::Rotation::Z,-3600,3600,0,0.01
--#define EULER XYZ Euler=5,XZY Euler=7,YXZ Euler=11,YZX Euler=15,ZXY Euler=19,ZYX Euler=21
--#define ROTATION_MODES Quaternion=0,Axis Angle=1,${EULER}
local xform_rotation_mode = 21 --select@xform_rotation_mode:Transform::Rotation::Mode=21,${ROTATION_MODES}
--trackgroup@xform_rotation_x,xform_rotation_y,xform_rotation_z:Group::Transform::Rotation
--separator:Scale
local xform_scale_x = 100.0 --track@xform_scale_x:Transform::Scale::X,-10000,10000,100,0.01
local xform_scale_y = 100.0 --track@xform_scale_y:Transform::Scale::Y,-10000,10000,100,0.01
local xform_scale_z = 100.0 --track@xform_scale_z:Transform::Scale::Z,-10000,10000,100,0.01
--trackgroup@xform_scale_x,xform_scale_y,xform_scale_z:Group::Transform::Scale
--separator:Compositing
--#define DARKEN Darken=7,Multiply=3,Linear Burn=10
--#define LIGHTEN Lighten=6,Screen=4,Linear Dodge (Add)=1
--#define CONTRAST Overlay=5,Linear Light=11
--#define COMPARATIVE Difference=12,Subtract=2
--#define HSL Color=9,Luminosity=8
--#define BLEND_MODES Normal=0,${DARKEN},${LIGHTEN},${CONTRAST},${COMPARATIVE},${HSL}
local xform_blend_mode = 0 --select@xform_blend_mode:Transform::Compositing::Blend Mode,${BLEND_MODES}
local xform_opacity = 100.0 --track@xform_opacity:Transform::Compositing::Opacity,0,100,100,0.01
--separator:Target
local xform_target_local_space = true --checksection@xform_target_local_space:Transform::Target::Local Space,true
local xform_target_world_space = false --checksection@xform_target_world_space:Transform::Target::World Space,false
--group:Tint,false
local tint_color = nil --color@tint_color:Tint::Color,nil
local tint_opacity = 100.0 --track@tint_opacity:Tint::Opacity,0,100,100,0.01
--group:Additional Options,false
local influence = 100.0 --track@influence:Influence,0,100,100,0.01

if obj.num < 2 then
    print("@error", "Enable Multi Object to use this script")
    return
end

do
    local _ = obj.setanchor("xform_position_x,xform_position_y,xform_position_z", 0, "line", "xyz")

    --#include "utilities.lua"
    local utils = require("utilities")
    local lerp = utils.lerp

    local buffer

    do
        buffer = require("string.buffer").new()
    end

    local ID = obj.effect_id
    local KEY_COUNT = "8973f111-5db5-4890-907d-52539fe55570-" .. ID

    local getvalue = obj.getvalue
    local INDEX, NUM, LAYER, FPS, TIME = obj.index, obj.num, obj.layer, obj.framerate, obj.time

    xform_scale_x = xform_scale_x * 0.01
    xform_scale_y = xform_scale_y * 0.01
    xform_scale_z = xform_scale_z * 0.01
    xform_opacity = xform_opacity * 0.01

    tint_opacity = tint_opacity * 0.01

    influence = influence * 0.01

    local text = getvalue(LAYER, "テキスト", "テキスト") --[[@as string | nil]]
    if text == nil then
        print("@error", "'テキスト' effect was not found in the source")
        return
    end

    local content = INDEX == 0 and text:gsub("<.->", "") or nil

    local i, n = INDEX, NUM

    local c
    if INDEX == 0 then
        local utf8 = obj.module("UTF8@${PROJECT_NAME}")

        c = utf8.count(content, true)
        global[KEY_COUNT] = tostring(c)
    else
        c = tonumber(global[KEY_COUNT])
    end

    if type(c) == "number" then
        if n % c == 0 then
            i = math.floor(i * c / n)
            n = c
        end
    else
        print("@warn", "Shared count value is missing or corrupted")
    end

    if INDEX == NUM - 1 then
        global[KEY_COUNT] = nil
    end

    local offset = { 0.0, 0.0 }
    if kerning_mode == 1 then
        local KEY_KERNING = "308494ea-bd57-4fbf-9573-458df515646c-" .. ID

        local t
        if INDEX == 0 then
            local kerning = obj.module("Kerning@${PROJECT_NAME}")

            local alignments = {
                ["左寄せ[上]"] = 0,
                ["中央揃え[上]"] = 1,
                ["右寄せ[上]"] = 2,
                ["左寄せ[中]"] = 3,
                ["中央揃え[中]"] = 4,
                ["右寄せ[中]"] = 5,
                ["左寄せ[下]"] = 6,
                ["中央揃え[下]"] = 7,
                ["右寄せ[下]"] = 8,
                ["縦書 上寄[右]"] = 9,
                ["縦書 中央[右]"] = 10,
                ["縦書 下寄[右]"] = 11,
                ["縦書 上寄[中]"] = 12,
                ["縦書 中央[中]"] = 13,
                ["縦書 下寄[中]"] = 14,
                ["縦書 上寄[左]"] = 15,
                ["縦書 中央[左]"] = 16,
                ["縦書 下寄[左]"] = 17,
            }

            t = {
                kerning.shift(
                    obj.id,
                    content,
                    getvalue(LAYER, "テキスト", "サイズ"),
                    getvalue(LAYER, "テキスト", "フォント"),
                    alignments[getvalue(LAYER, "テキスト", "文字揃え")],
                    getvalue(LAYER, "テキスト", "B") ~= "0",
                    getvalue(LAYER, "テキスト", "I") ~= "0"
                ),
            }
            global[KEY_KERNING] = buffer:reset():encode(t):get()
        else
            t = buffer:set(global[KEY_KERNING]):decode()
        end

        if type(t) == "table" and #t == n then
            offset = t[i + 1]
        else
            print("@warn", "Shared kerning offset table is missing or corrupted")
        end

        if INDEX == NUM - 1 then
            global[KEY_KERNING] = nil
        end
    end

    local is_matched = true
    if filter_regex_pattern ~= "" then
        local KEY_REGEX = "db5638b7-59ab-4c53-8471-772fc3b03366-" .. ID

        local t = {}
        if INDEX == 0 then
            local regex = obj.module("Regex@${PROJECT_NAME}")

            for _, m in ipairs({ regex.mark(ID, content, filter_regex_pattern, filter_capture_group) }) do
                if not m[2] then
                    t[#t + 1] = m[1]
                end
            end
            global[KEY_REGEX] = buffer:reset():encode(t):get()
        else
            t = buffer:set(global[KEY_REGEX]):decode()
        end

        if type(t) == "table" and #t == n then
            is_matched = t[i + 1]
        else
            print("@warn", "Shared regex filter table is missing or corrupted")
        end

        if INDEX == NUM - 1 then
            global[KEY_REGEX] = nil
        end
    end

    if is_matched then
        local vector = obj.module("Vector@${PROJECT_NAME}")
        local rotate = vector.rotate

        if xform_target_world_space then
            local v = {
                (obj.ox - xform_pivot_x * influence) * (1.0 + (xform_scale_x - 1.0) * influence),
                (obj.oy - xform_pivot_y * influence) * (1.0 + (xform_scale_y - 1.0) * influence),
                (obj.oz - xform_pivot_z * influence) * (1.0 + (xform_scale_z - 1.0) * influence),
            }

            v = rotate(
                influence,
                xform_rotation_mode,
                xform_rotation_w,
                xform_rotation_x,
                xform_rotation_y,
                xform_rotation_z,
                v
            )

            obj.ox = v[1]
            obj.oy = v[2]
            obj.oz = v[3]
        end

        if xform_target_local_space then
            local rx, ry, rz = rotate(
                influence,
                xform_rotation_mode,
                xform_rotation_w,
                xform_rotation_x,
                xform_rotation_y,
                xform_rotation_z
            )

            obj.cx = obj.cx + xform_pivot_x * influence
            obj.cy = obj.cy + xform_pivot_y * influence
            obj.cz = obj.cz + xform_pivot_z * influence
            obj.rx = obj.rx + rx
            obj.ry = obj.ry + ry
            obj.rz = obj.rz + rz
            obj.sx = lerp(obj.sx, obj.sx * xform_scale_x, influence)
            obj.sy = lerp(obj.sy, obj.sy * xform_scale_y, influence)
            obj.sz = lerp(obj.sz, obj.sz * xform_scale_z, influence)
        end

        if xform_target_local_space or xform_target_world_space then
            obj.ox = obj.ox + (xform_position_x + offset[1]) * influence
            obj.oy = obj.oy + (xform_position_y + offset[2]) * influence
            obj.oz = obj.oz + xform_position_z * influence
        end

        obj.alpha = lerp(obj.alpha, obj.alpha * xform_opacity, influence)

        ---@diagnostic disable-next-line: param-type-mismatch
        obj.setoption("blend", xform_blend_mode)

        if tint_color ~= nil then
            local r, g, b = RGB(tint_color)
            obj.pixelshader(
                "tint@Motion@${SCRIPT_NAME}",
                "object",
                "object",
                { r / 255.0, g / 255.0, b / 255.0, 1.0, tint_opacity * influence }
            )
        end
    elseif should_limit_fx then
        obj.draw()
    end
end
