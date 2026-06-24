--@Motion

--require:${PROJECT_REQUIRES_AVIUTL2}
--information:Motion@${SCRIPT_NAME} v${PROJECT_VERSION} by ${PROJECT_AUTHOR}
--label:${LABEL}

local timing = 0 --select@timing:Timing,Automatic=0,Manual=1
local duration = 24.0 --track@duration:Duration,-1000,1000,24,0.001,---
local offset = 0.0 --track@offset:Offset,-1000,1000,0,0.001
--group:Motion
--#define BASED_ON Whole=-2,Objects=-1,Characters=0,Characters Excluding Spaces=1,Words=2,Lines=3
local motion_based_on = 0 --select@motion_based_on:Motion::Based On,${BASED_ON}
local motion_order = 0 --select@motion_order:Motion::Order,Forward=0,Reverse=1,Random=2
local motion_overlap = 100.0 --track@motion_overlap:Motion::Overlap,0,100,100,0.01
local motion_softness = 0.0 --track@motion_softness:Motion::Softness,0,100,100,0.01
--track0:Motion::Curve,0,1,0,0.001
local motion_cutoff = 0.0 --track@motion_cutoff:Motion::Cutoff,-10000,10000,0,0.001
local motion_should_mask = false --checksection@motion_should_mask:Motion::Mask,false,false
--group:Lyric,false
local lyric_distance = 0.0 --track@lyric_distance:Lyric::Distance,0,10000,0,0.01
local lyric_stretch = 0.0 --track@lyric_stretch:Lyric::Stretch,-10,10,0,0.001
local lyric_jitter = 50.0 --track@lyric_jitter:Lyric::Jitter,0,100,50,0.01
--separator:Layout
local lyric_shape = 0 --select@lyric_shape:Lyric::Layout::Shape,Circle=0,Arc=1
local lyric_angle = 0.0 --track@lyric_angle:Lyric::Layout::Angle,-3600,3600,0,0.01
local lyric_sweep = 270.0 --track@lyric_sweep:Lyric::Layout::Sweep,-3600,3600,270,0.01
local lyric_divisions = 4 --track@lyric_divisions:Lyric::Layout::Divisions,1,36,4,1
local lyric_selection = 0 --select@lyric_selection:Lyric::Layout::Selection,Random=0,Clockwise=1,Counter-Clockwise=2
--separator:Dynamics
local lyric_peak = 1.0 --track@lyric_peak:Lyric::Dynamics::Peak,0,100,1,0.001
local lyric_should_overshoot = false --checksection@lyric_should_overshoot:Lyric::Dynamics::Overshoot,false,false
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
local xform_scale_x = 100.0 --track@xform_scale_x:Transform::Scale::X,0,10000,100,0.01
local xform_scale_y = 100.0 --track@xform_scale_y:Transform::Scale::Y,0,10000,100,0.01
local xform_scale_z = 100.0 --track@xform_scale_z:Transform::Scale::Z,0,10000,100,0.01
--trackgroup@xform_scale_x,xform_scale_y,xform_scale_z:Group::Transform::Scale
--separator:Compositing
local xform_opacity = 100.0 --track@xform_opacity:Transform::Compositing::Opacity,0,100,100,0.01
--separator:Target
local xform_target_local_space = true --checksection@xform_target_local_space:Transform::Target::Local Space,true,false
local xform_target_world_space = false --checksection@xform_target_world_space:Transform::Target::World Space,false,false
--group:Blink,false
local blink_duration = 0.0 --track@blink_duration:Blink::Duration,0,10000,0,0.001,---
--separator:Opacity
local blink_opacity_steps = 2 --track@blink_opacity_steps:Blink::Opacity::Steps,2,128,2,1
local blink_opacity_min = 0.0 --track@blink_opacity_min:Blink::Opacity::Minimum,0,100,0,0.01
local blink_opacity_max = 100.0 --track@blink_opacity_max:Blink::Opacity::Maximum,0,100,100,0.01
--separator:Scale
local blink_scale_steps = 2 --track@blink_scale_steps:Blink::Scale::Steps,2,128,2,1
local blink_scale_min = 100.0 --track@blink_scale_min:Blink::Scale::Minimum,-10000,10000,100,0.01
local blink_scale_max = 100.0 --track@blink_scale_max:Blink::Scale::Maximum,-10000,10000,100,0.01
--separator:Edge Detection
local blink_edge_intensity = 100.0 --track@blink_edge_intensity:Blink::Edge Detection::Intensity,0,1000,100,0.01
local blink_edge_threshold = -100.0 --track@blink_edge_threshold:Blink::Edge Detection::Threshold,-100,100,-100,0.01
--separator:Characters
local blink_characters_pool = "" --string@blink_characters_pool:Blink::Characters::Pool,
local blink_characters_font = "Noto Sans JP Black" --font@blink_characters_font:Blink::Characters::Font,Noto Sans JP Black
local blink_characters_scale = 100.0 --track@blink_characters_scale:Blink::Characters::Scale,0,1000,100,0.01
--group:Tint,false
local tint_source = 0 --select@tint_source:Tint::Source,Image=0,Layer=1
local tint_image = "" --file@tint_image:Tint::Image,""
local tint_layer = 0 --track@tint_layer:Tint::Layer,-100,100,0,1,---
--group:Effect,false
local effect_params = "" --text@effect_params:Effect::Parameters,
--group:Echo,false
local echo_count = 0 --track@echo_count:Echo::Count,0,100,0,1,---
local echo_interval = 0.0 --track@echo_interval:Echo::Interval,-100,100,-1,0.001
local echo_decay = 50.0 --track@echo_decay:Echo::Decay,0,100,50,0.01
local echo_composite = 1 --select@echo_composite:Echo::Composite=1,Above=0,Below=1
--group:Additional Options,false
local unit = 0 --select@unit:Unit,Frames=0,Seconds=1
local layer_reference = 0 --select@layer_reference:Layer Reference,Absolute=0,Relative=1
local seed = 0 --track@seed:Seed,-10000,10000,0,1
local should_highlight = false --check@should_highlight:Highlight Overlap,false
--[[pixelshader@tint:
--#include <tint.hlsl>
]]
--[[pixelshader@map:
--#include <map.hlsl>
]]
--[[pixelshader@alpha_mask:
--#include <alpha_mask.hlsl>
]]
--[[pixelshader@mask:
--#include <mask.hlsl>
]]

if obj.num < 1 or obj.framerate == 0 then
    print("@error", "Framerate is zero or index is out of bounds")
    return
end

do
    local _ = obj.setanchor("xform_position_x,xform_position_y,xform_position_z", 0, "line", "xyz")

    --#include "utilities.lua"
    local utils = require("utilities")
    local lerp, clamp, copy_xform, stop = utils.lerp, utils.clamp, utils.copy_xform, utils.stop

    local buffer

    do
        buffer = require("string.buffer").new()
    end

    local eps = 1.0e-4

    local max, min, abs, rad, cos, sin, floor = math.max, math.min, math.abs, math.rad, math.cos, math.sin, math.floor
    local random, randomseed = math.random, math.randomseed
    local getvalue, getinfo, getoption, effect = obj.getvalue, obj.getinfo, obj.getoption, obj.effect
    local copybuffer, pixelshader = obj.copybuffer, obj.pixelshader
    local ID, INDEX, NUM, LAYER = obj.effect_id, obj.index, obj.num, obj.layer
    local FPS, TIME, TOTALTIME = obj.framerate, obj.time, obj.totaltime
    local spf = 1.0 / FPS

    offset = duration < 0.0 and -offset or offset

    motion_overlap = motion_overlap * 0.01
    motion_softness = motion_softness * 0.01

    lyric_jitter = lyric_jitter * 0.01
    lyric_angle = rad(lyric_angle)
    lyric_sweep = rad(lyric_sweep)

    xform_scale_x = max(xform_scale_x * 0.01, eps)
    xform_scale_y = max(xform_scale_y * 0.01, eps)
    xform_scale_z = max(xform_scale_z * 0.01, eps)
    xform_opacity = clamp(xform_opacity * 0.01, eps, 1.0)

    blink_opacity_min = blink_opacity_min * 0.01
    blink_opacity_max = blink_opacity_max * 0.01
    blink_scale_min = blink_scale_min * 0.01
    blink_scale_max = blink_scale_max * 0.01

    tint_layer = layer_reference == 0 and max(tint_layer, 0) or max(LAYER + tint_layer, 0)

    echo_count = echo_count + 1
    echo_decay = echo_decay * 0.01

    local should_load_lut = false
    if tint_source == 0 then
        should_load_lut = tint_image ~= ""
    elseif tint_source == 1 then
        if tint_layer > 0 and tint_layer ~= LAYER then
            if not getvalue("layer" .. tint_layer) then
                local frame = getvalue("frame_s") + obj.frame
                print("@warn", "No object found in Layer " .. tint_layer .. " at Frame " .. frame)
            else
                should_load_lut = true
            end
        end
    end

    if seed >= 0 then
        randomseed(seed)
        seed = LAYER + random(2147483647) + 1
    else
        seed = -seed
    end

    if unit == 0 then
        motion_cutoff = motion_cutoff * spf
        blink_duration = blink_duration * spf
        echo_interval = echo_interval * spf

        if timing == 0 then
            duration = duration * spf
            offset = offset * spf
        end
    end

    if abs(duration) < eps then
        return
    end

    local motion, mask

    do
        local utf8 = obj.module("UTF8@${PROJECT_NAME}")

        local CACHE_LUT = "cache:a09d04ba-6a95-4940-b8f4-41f9d7483817-" .. ID

        local text = getvalue(LAYER, "テキスト", "テキスト") --[[@as string | nil]]

        if should_load_lut then
            local ok, e = pcall(function()
                if not copybuffer("cache:tmp", "object") then
                    error("Failed to copy buffer")
                end

                local xform = {}
                copy_xform(xform, obj)

                if tint_source == 0 then
                    if not obj.load("image", tint_image) then
                        if not copybuffer("object", "cache:tmp") then
                            error("Failed to copy buffer")
                        end

                        error("Failed to load image")
                    end
                elseif tint_source == 1 then
                    if not obj.load("layer", tint_layer, true) then
                        if not copybuffer("object", "cache:tmp") then
                            error("Failed to copy buffer")
                        end

                        error("Failed to load layer data")
                    end
                end

                if not copybuffer(CACHE_LUT, "object") then
                    error("Failed to copy buffer")
                end

                if not copybuffer("object", "cache:tmp") then
                    error("Failed to copy buffer")
                end

                copy_xform(obj, xform)
            end)

            if not ok then
                stop(e)
                return
            end
        end

        local get_time

        do
            local modf = math.modf

            local n = getoption("section_num")

            get_time = function(index)
                local i, f = modf(index)

                if i < 0 then
                    i = n + i
                    f = 1.0 + f
                end

                return getvalue("time", (getvalue("time", 0.0, i + 1) - getvalue("time", 0.0, i)) * f, i)
            end
        end

        local function interpolate(t)
            t = clamp(t, 0.0, 1.0)

            local st = getvalue(0, 0.0)
            local range = getvalue(0, 0.0, -1) - st

            return abs(range) > eps and (getvalue(0, TOTALTIME * t) - st) / range or t
        end

        local progress

        if duration < 0.0 then
            progress = function(t, i, n)
                t = t - TOTALTIME - duration
                local d = duration * (motion_overlap + (1.0 - motion_overlap) / n) * max(motion_softness, eps)

                if timing == 1 then
                    i = (get_time(i - n) - TOTALTIME - duration) * (d + duration) / duration
                else
                    i = -duration * i / n - min(d, 0.0)
                end

                local p = t / duration + (t - i) / d
                local w = 1.0 - interpolate(p)

                return (duration * i) / (duration + d) - t, -w
            end
        else
            progress = function(t, i, n)
                local d = duration * (motion_overlap + (1.0 - motion_overlap) / n) * max(motion_softness, eps)

                if timing == 1 then
                    i = get_time(i) * (d + duration) / duration
                else
                    i = duration * i / n - min(d, 0.0)
                end

                local p = t / duration + (t - i) / d
                local w = 1.0 - interpolate(p)

                return t - (duration * i) / (duration + d), w
            end
        end

        local apply_xform

        do
            local vector = obj.module("Vector@${PROJECT_NAME}")
            local rotate = vector.rotate

            apply_xform = function(w)
                if xform_target_world_space then
                    local v = {
                        (obj.ox - xform_pivot_x * w) * (xform_scale_x ^ w),
                        (obj.oy - xform_pivot_y * w) * (xform_scale_y ^ w),
                        (obj.oz - xform_pivot_z * w) * (xform_scale_z ^ w),
                    }

                    v = rotate(
                        w,
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
                    local rx, ry, rz = xform_rotation_x, xform_rotation_y, xform_rotation_z
                    rx, ry, rz = rotate(w, xform_rotation_mode, xform_rotation_w, rx, ry, rz)

                    obj.cx = obj.cx + xform_pivot_x * w
                    obj.cy = obj.cy + xform_pivot_y * w
                    obj.cz = obj.cz + xform_pivot_z * w
                    obj.rx = obj.rx + rx
                    obj.ry = obj.ry + ry
                    obj.rz = obj.rz + rz
                    obj.sx = obj.sx * (xform_scale_x ^ w)
                    obj.sy = obj.sy * (xform_scale_y ^ w)
                    obj.sz = obj.sz * (xform_scale_z ^ w)
                end

                if xform_target_local_space or xform_target_world_space then
                    obj.ox = obj.ox + xform_position_x * w
                    obj.oy = obj.oy + xform_position_y * w
                    obj.oz = obj.oz + xform_position_z * w
                end

                obj.alpha = lerp(obj.alpha, obj.alpha * xform_opacity, abs(w))
            end
        end

        local apply_lyric, apply_blink

        do
            local hash = obj.module("Hash@${PROJECT_NAME}")
            local hash4d = hash.hash4d

            if lyric_distance > eps then
                apply_lyric = function(i, n, t, w)
                    local hx, hy, _, _ = hash4d(i, n, seed)

                    local r = lerp(lyric_distance, lyric_distance * hx, lyric_jitter) * w
                    local s = lerp(lyric_stretch, lyric_stretch * hx, lyric_jitter) * abs(w)

                    local d = 0.0
                    if lyric_shape == 0 then
                        d = 2.0 * math.pi / lyric_divisions
                    elseif lyric_divisions ~= 1 then
                        d = lyric_sweep / (lyric_divisions - 1)
                    end

                    local angle = lyric_angle
                    if lyric_selection == 0 then
                        angle = angle + d * floor(hy * lyric_divisions)
                    elseif lyric_selection == 1 then
                        angle = angle + d * i
                    elseif lyric_selection == 2 then
                        angle = angle - d * i
                    end

                    local x, y = cos(angle), sin(angle)
                    obj.ox = obj.ox + r * x
                    obj.oy = obj.oy + r * y
                    obj.sx = obj.sx * (2.0 ^ (s * abs(x)))
                    obj.sy = obj.sy * (2.0 ^ (s * abs(y)))

                    if t < spf then
                        local p = 2.0 ^ lyric_peak
                        obj.sx = obj.sx * p
                        obj.sy = obj.sy * p
                    elseif lyric_should_overshoot and t < 2.0 * spf then
                        local p = 2.0 ^ (-lyric_peak * 0.5)
                        obj.sx = obj.sx * p
                        obj.sy = obj.sy * p
                    end
                end
            end

            if blink_duration > eps then
                local should_blink_opacity = blink_opacity_max - blink_opacity_min > eps
                local should_blink_scale = blink_scale_max - blink_scale_min > eps
                local should_edge_detect = blink_edge_threshold > -100.0 + eps

                local alignment, chars

                if blink_characters_pool ~= "" then
                    if text ~= nil then
                        local styles = {
                            ["標準文字"] = 0,
                            ["影付き文字"] = 1,
                            ["影付き文字(薄)"] = 2,
                            ["縁取り文字"] = 3,
                            ["縁取り文字(細)"] = 4,
                            ["縁取り文字(太)"] = 5,
                            ["縁取り文字(角)"] = 6,
                        }

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

                        obj.setfont(
                            blink_characters_font,
                            obj.getvalue(LAYER, "テキスト", "サイズ") * blink_characters_scale * 0.01,
                            styles[obj.getvalue(LAYER, "テキスト", "文字装飾")],
                            obj.getvalue(LAYER, "テキスト", "文字色") --[[@as number]],
                            obj.getvalue(LAYER, "テキスト", "影・縁色") --[[@as number]],
                            obj.getvalue(LAYER, "テキスト", "B") ~= "0",
                            obj.getvalue(LAYER, "テキスト", "I") ~= "0",
                            obj.getvalue(LAYER, "テキスト", "字間") --[[@as number]],
                            obj.getvalue(LAYER, "テキスト", "行間") --[[@as number]]
                        )

                        alignment = alignments[obj.getvalue(LAYER, "テキスト", "文字揃え")]
                    else
                        obj.setfont(
                            blink_characters_font,
                            max(obj.w, obj.h) * blink_characters_scale * 0.01,
                            0,
                            0xffffff,
                            0x000000,
                            false,
                            false,
                            0,
                            0
                        )

                        alignment = 4
                    end

                    chars = utf8.split(blink_characters_pool, true)
                end

                apply_blink = function(i, n, t)
                    if blink_characters_pool ~= "" then
                        local hx, _, _, _ = hash4d(i, n, seed + 1, t * FPS * 100.0, 1, #chars)

                        local xform = {}
                        copy_xform(xform, obj)

                        obj.load("text", chars[hx], 0.0, 0.0, alignment)

                        copy_xform(obj, xform)
                    end

                    local hx, hy, hz, _ = hash4d(i, n, seed + 2, t * FPS * 100.0)

                    if should_blink_opacity then
                        local r = floor(hx * blink_opacity_steps) / (blink_opacity_steps - 1)
                        obj.alpha = lerp(blink_opacity_min, blink_opacity_max, r)
                    end

                    if should_blink_scale then
                        local r = floor(hy * blink_scale_steps) / (blink_scale_steps - 1)
                        local scale = lerp(blink_scale_min, blink_scale_max, r)
                        obj.sx = scale
                        obj.sy = scale
                        obj.sz = scale
                    end

                    if should_edge_detect and hz < 0.5 then
                        local ok, e = pcall(function()
                            if not copybuffer("cache:tmp", "object") then
                                error("Failed to copy buffer")
                            end

                            effect(
                                "エッジ抽出",
                                "強さ",
                                blink_edge_intensity,
                                "しきい値",
                                blink_edge_threshold,
                                "輝度エッジを抽出",
                                0,
                                "透明度エッジを抽出",
                                1
                            )

                            pixelshader("alpha_mask", "cache:tmp", "object", { 0.0 }, "mask")

                            if not copybuffer("object", "cache:tmp") then
                                error("Failed to copy buffer")
                            end
                        end)

                        if not ok then
                            stop(e)
                            return
                        end
                    end
                end
            end
        end

        local apply_fx

        if effect_params ~= "" then
            local KEY_FX = "99c01c07-d684-4b55-a6ca-7cd4bb9e390c-" .. ID

            local data
            if INDEX == 0 then
                local toml = obj.module("Toml@${PROJECT_NAME}")

                data = { toml.parse("motion::effect", effect_params) }
                global[KEY_FX] = buffer:reset():encode(data):get()
            else
                data = buffer:set(global[KEY_FX]):decode()
            end

            if INDEX == NUM - 1 then
                global[KEY_FX] = nil
            end

            if type(data) == "table" then
                local env = {
                    _VERSION = _VERSION,
                    ipairs = ipairs,
                    pairs = pairs,
                    next = next,
                    select = select,
                    unpack = unpack,
                    type = type,
                    tonumber = tonumber,
                    tostring = tostring,
                    pcall = pcall,
                    xpcall = xpcall,
                    print = print,
                    math = math,
                    string = string,
                    table = table,
                    bit = bit,
                    setfont = setfont,
                    rand = rand,
                    rand1 = rand1,
                    RGB = RGB,
                    HSV = HSV,
                    OR = OR,
                    AND = AND,
                    XOR = XOR,
                    SHIFT = SHIFT,
                    rotation = rotation,
                    debug_print = debug_print,
                    obj = obj,
                    weight = nil,
                }

                apply_fx = function(w)
                    env.weight = w

                    local params
                    for i, t in ipairs(data) do
                        if i % 2 == 1 then
                            params = {}
                            params[#params + 1] = t
                        else
                            for k, v in pairs(t) do
                                local p = k:match("^lua:(.*)$")
                                if p ~= nil and p ~= "" then
                                    local f, e = load(v, nil, "t", env)
                                    if type(f) == "function" then
                                        local ok, result = pcall(f)
                                        if ok then
                                            params[#params + 1] = p
                                            params[#params + 1] = result
                                        else
                                            print("@warn", "Lua execution failed for the FX key '" .. p .. "'")
                                            print(result)
                                        end
                                    else
                                        print("@warn", "Lua syntax error detected in the FX key '" .. p .. "'")
                                        print(e)
                                    end
                                else
                                    params[#params + 1] = k
                                    params[#params + 1] = tonumber(v) or v
                                end
                            end
                            effect(unpack(params))
                        end
                    end
                end
            else
                print("@warn", "Shared FX data table is missing or corrupted")
            end
        end

        do
            local i, n = INDEX, NUM

            if NUM > 1 then
                local content

                if motion_based_on >= 0 then
                    local KEY_COUNT = "58172aa6-e0d5-4503-aa68-984427e8f04f-" .. ID

                    if text ~= nil then
                        local c
                        if INDEX == 0 then
                            content = text:gsub("<.->", "")

                            c = utf8.count(content, true)
                            global[KEY_COUNT] = tostring(c)
                        else
                            c = tonumber(global[KEY_COUNT])
                        end

                        if type(c) == "number" then
                            if n % c == 0 then
                                i = floor(i * c / n)
                                n = c
                            end
                        else
                            print("@warn", "Shared count value is missing or corrupted")
                        end

                        if INDEX == NUM - 1 then
                            global[KEY_COUNT] = nil
                        end
                    end
                end

                if text ~= nil and motion_based_on > 0 then
                    local KEY_GROUP = "a7d6bd98-24a1-480a-bf02-679a2c80c83d-" .. ID

                    local t
                    if INDEX == 0 then
                        local regex = obj.module("Regex@${PROJECT_NAME}")

                        t = {}

                        local j, id, pattern
                        if motion_based_on == 1 then
                            j = -1
                            id = -1
                            pattern = [=[[^\s\v\x85\pZ]]=]
                        elseif motion_based_on == 2 then
                            j = 0
                            id = -2
                            pattern = [=[[\s\v\x85\pZ]]=]
                        else
                            j = 0
                            id = -3
                            pattern = "\\n"
                        end

                        for _, m in ipairs({ regex.mark(id, content, pattern) }) do
                            if m[1] then
                                j = j + 1
                            end

                            if not m[2] then
                                t[#t + 1] = max(j, 0)
                            end
                        end

                        global[KEY_GROUP] = buffer:reset():encode(t):get()
                    else
                        t = buffer:set(global[KEY_GROUP]):decode()
                    end

                    if type(t) == "table" and #t == n then
                        i = t[i + 1]
                        n = t[n] + 1
                    else
                        print("@warn", "Shared motion group table is missing or corrupted")
                    end

                    if INDEX == NUM - 1 then
                        global[KEY_GROUP] = nil
                    end
                elseif motion_based_on == -2 then
                    i = 0
                    n = 1
                end
            end

            if motion_order == 1 then
                i = n - 1 - i
            elseif motion_order == 2 then
                local KEY_ORDER = "66d63ef0-5c82-46b4-8989-12fa433c66e9-" .. ID

                local t
                if INDEX == 0 then
                    randomseed(seed)

                    t = {}

                    for j = 1, n do
                        t[j] = j - 1
                    end

                    for j = n, 1, -1 do
                        local k = random(1, j)
                        t[j], t[k] = t[k], t[j]
                    end

                    global[KEY_ORDER] = buffer:reset():encode(t):get()
                else
                    t = buffer:set(global[KEY_ORDER]):decode()
                end

                if type(t) == "table" and #t == n then
                    i = t[i + 1]
                else
                    print("@warn", "Shared motion order table is missing or corrupted")
                end

                if INDEX == NUM - 1 then
                    global[KEY_ORDER] = nil
                end
            end

            if timing == 1 then
                local sections = getoption("section_num")
                if sections < n then
                    print("@warn", (n - sections) .. " more sections are required to map all units")
                end

                if duration < 0.0 then
                    duration = get_time(-n - 1) - TOTALTIME

                    if -duration < eps then
                        return
                    end

                    local amount = abs(offset)

                    if amount >= eps then
                        local base = -1 - i
                        amount = get_time(base) - get_time(-amount + base)
                        offset = offset < 0.0 and -amount or amount
                    end
                else
                    duration = get_time(n)

                    if duration < eps then
                        return
                    end

                    local amount = abs(offset)

                    if amount >= eps then
                        amount = get_time(amount + i) - get_time(i)
                        offset = offset < 0.0 and -amount or amount
                    end
                end
            end

            motion = function(dt, idx)
                local t, w = progress(clamp(TIME + dt - offset, 0.0, TOTALTIME), i, n)

                apply_xform(w)

                if apply_lyric ~= nil then
                    apply_lyric(i, n, t, w)
                end

                if apply_blink ~= nil and t >= 0.0 and t < blink_duration then
                    apply_blink(i, n, t)
                end

                if should_load_lut then
                    pixelshader(
                        "map",
                        "object",
                        { "object", CACHE_LUT },
                        { abs(w), idx / max((echo_count - 1), 1) },
                        "copy",
                        "clamp"
                    )
                end

                if apply_fx ~= nil then
                    apply_fx(w)
                end

                if t < motion_cutoff then
                    obj.alpha = eps
                end

                if should_highlight and not getinfo("saving") then
                    local x = 1.0 + (n - 1) * motion_overlap
                    pixelshader("tint", "object", "object", {
                        1.0,
                        0.0,
                        0.0,
                        1.0,
                        max(0.0, (x - i) / x),
                    })
                end
            end
        end

        if motion_should_mask then
            local W, H = obj.w, obj.h
            local texel = 1.0 / H
            local aspect = W * texel

            mask = function(f)
                local cx0, cy0, x0, y0, r0, sx0, sy0 = obj.cx, obj.cy, obj.ox, obj.oy, rad(obj.rz), obj.sx, obj.sy

                f()

                local cx1, cy1, x1, y1, r1, sx1, sy1 = obj.cx, obj.cy, obj.ox, obj.oy, rad(obj.rz), obj.sx, obj.sy

                sx0 = sx0 < -eps and 1.0 / sx0 or 1.0 / max(sx0, eps)
                sy0 = sy0 < -eps and 1.0 / sy0 or 1.0 / max(sy0, eps)

                local dx, dy, dr = (x1 - x0) * texel, (y1 - y0) * texel, r1 - r0

                local x, y = cos(dr), sin(dr)
                local m00 = x * sx1 * sx0
                local m01 = -y * sy1 * sx0
                local m10 = y * sx1 * sy0
                local m11 = x * sy1 * sy0

                x, y = cos(r0), sin(r0)
                local m02 = (x * dx + y * dy) * sx0
                local m12 = (-y * dx + x * dy) * sy0

                local px, py = (cx0 - cx1) * texel, (cy0 - cy1) * texel

                pixelshader(
                    "mask",
                    "object",
                    {},
                    { m00, m10, 0.0, 0.0, m01, m11, 0.0, 0.0, m02, m12, 1.0, 0.0, px, py, aspect },
                    "mask"
                )
            end
        end
    end

    if echo_count > 1 then
        local CACHE_IMAGE = "cache:cc2b9a9b-89d5-4481-b8f0-58ca17cd499f-" .. ID

        if not copybuffer(CACHE_IMAGE, "object") then
            stop("Failed to copy buffer")
            return
        end

        local xform = {}
        copy_xform(xform, obj)

        local i = -1
        obj.multiobject(echo_count, function()
            i = i + 1

            if not copybuffer("object", CACHE_IMAGE) then
                stop("Failed to copy buffer")
                return
            end

            copy_xform(obj, xform)

            local j = echo_composite == 0 and i or echo_count - i - 1

            local dt = echo_interval * j

            if motion_should_mask then
                mask(function()
                    motion(dt, j)
                end)
            else
                motion(dt, j)
            end

            obj.alpha = obj.alpha * (echo_decay ^ j)

            return clamp(TIME + dt, 0.0, TOTALTIME) - TIME
        end)
    else
        if motion_should_mask then
            mask(function()
                motion(0.0, 0.0)
            end)
        else
            motion(0.0, 0.0)
        end
    end
end
