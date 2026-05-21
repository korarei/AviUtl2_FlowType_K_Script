--@Motion

--require:${PROJECT_REQUIRES_AVIUTL2}
--information:Motion@${SCRIPT_NAME} v${PROJECT_VERSION} by ${PROJECT_AUTHOR}
--label:${LABEL}

local timing = 0 --select@timing:Timing,Automatic=0,Manual=1
local duration = 24.0 --track@duration:Duration,-1000,1000,24,0.001,---
--group:Motion
--#define BASED_ON Whole=-1,Characters=0,Characters Excluding Spaces=1,Words=2,Lines=3
local motion_based_on = 0 --select@motion_based_on:Motion::Based On,${BASED_ON}
local motion_order = 0 --select@motion_order:Motion::Order,Forward=0,Reverse=1,Random=2
local motion_overlap = 100.0 --track@motion_overlap:Motion::Overlap,0,100,100,0.01
local motion_softness = 0.0 --track@motion_softness:Motion::Softness,0,100,100,0.01
--track0:Motion::Curve,0,1,0,0.001
local motion_cutoff = 0.0 --track@motion_cutoff:Motion::Cutoff,-10000,10000,0,0.001
local motion_should_mask = false --checksection@motion_should_mask:Motion::Mask,false
--group:Lyric,false
local lyric_distance = 0.0 --track@lyric_distance:Lyric::Distance,0,10000,0,0.01
local lyric_stretch = 0.0 --track@lyric_stretch:Lyric::Stretch,-10,10,0,0.001
local lyric_jitter = 50.0 --track@lyric_jitter:Lyric::Jitter,0,100,50,0.01
--separator:Layout
local lyric_shape = 0 --select@lyric_shape:Lyric::Shape,Circle=0,Arc=1
local lyric_angle = 0.0 --track@lyric_angle:Lyric::Angle,-3600,3600,0,0.01
local lyric_sweep = 270.0 --track@lyric_sweep:Lyric::Sweep,-3600,3600,270,0.01
local lyric_divisions = 4 --track@lyric_divisions:Lyric::Divisions,1,36,4,1
local lyric_selection = 0 --select@lyric_selection:Lyric::Selection,Random=0,Clockwise=1,Counter-Clockwise=2
--separator:Dynamics
local lyric_peak = 1.0 --track@lyric_peak:Lyric::Peak,0,100,1,0.001
local lyric_should_overshoot = false --checksection@lyric_should_overshoot:Lyric::Overshoot,false
--group:Transform,false
--separator:Pivot Point
local xform_pivot_x = 0.0 --track@xform_pivot_x:Transform::Pivot::X,-100000,100000,0,0.01
local xform_pivot_y = 0.0 --track@xform_pivot_y:Transform::Pivot::Y,-100000,100000,0,0.01
local xform_pivot_z = 0.0 --track@xform_pivot_z:Transform::Pivot::Z,-100000,100000,0,0.01
--trackgroup@xform_pivot_x,xform_pivot_y,xform_pivot_z:Group::Transform::Pivot Point
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
local xform_rotation_mode = 21 --select@xform_rotation_mode:Transform::Rotation::Mode=21,Quaternion=0,Axis Angle=1,${EULER}
--trackgroup@xform_rotation_x,xform_rotation_y,xform_rotation_z:Group::Transform::Rotation
--separator:Scale
local xform_scale_x = 100.0 --track@xform_scale_x:Transform::Scale::X,0,10000,100,0.01
local xform_scale_y = 100.0 --track@xform_scale_y:Transform::Scale::Y,0,10000,100,0.01
local xform_scale_z = 100.0 --track@xform_scale_z:Transform::Scale::Z,0,10000,100,0.01
--trackgroup@xform_scale_x,xform_scale_y,xform_scale_z:Group::Transform::Scale
--separator:Compositing
local xform_opacity = 100.0 --track@xform_opacity:Transform::Compositing::Opacity,0,100,100,0.01
--separator:Target
local xform_target_local_space = true --checksection@xform_target_local_space:Transform::Target::Local Space,true
local xform_target_world_space = false --checksection@xform_target_world_space:Transform::Target::World Space,false
--group:Blink,false
local blink_duration = 0.0 --track@blink_duration:Blink::Duration,0,10000,0,0.001,---
local blink_steps = 2 --track@blink_steps:Blink::Steps,2,128,2,1
--separator:Opacity
local blink_opacity_min = 0.0 --track@blink_opacity_min:Blink::Opacity::Minimum,0,100,0,0.01
local blink_opacity_max = 100.0 --track@blink_opacity_max:Blink::Opacity::Maximum,0,100,100,0.01
--separator:Scale
local blink_scale_min = 100.0 --track@blink_scale_min:Blink::Scale::Minimum,-10000,10000,100,0.01
local blink_scale_max = 100.0 --track@blink_scale_max:Blink::Scale::Maximum,-10000,10000,100,0.01
--separator:Edge Detection
local blink_edge_intensity = 100.0 --track@blink_edge_intensity:Blink::Edge Detection::Intensity,0,1000,100,0.01
local blink_edge_threshold = -100.0 --track@blink_edge_threshold:Blink::Edge Detection::Threshold,-100,100,-100,0.01
--group:Tint,false
local tint_source = 0 --select@tint_source:Tint::Source,Image=0,Layer=1
local tint_image = "" --file@tint_image:Tint::Image,""
local tint_layer = 0 --track@tint_layer:Tint::Layer,-100,100,0,1,---
--group:Effect,false
local effect_params = "" --text@effect_params:Effect::Parameters,""
--group:Echo,false
local echo_interval = 0.0 --track@echo_interval:Echo::Interval,-100,100,-1,0.001
local echo_count = 1 --track@echo_count:Echo::Count,1,100,1,1
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

if obj.index >= obj.num or obj.framerate == 0 then
    print("@error", "Framerate is zero or index is out of bounds")
    return
end

do
    local _ = obj.setanchor("xform_position_x,xform_position_y,xform_position_z", 0, "line", "xyz")

    --#include "utilities.lua"
    local utils = require("utilities")
    local lerp, clamp, copy_xform = utils.lerp, utils.clamp, utils.copy_xform

    local eps = 1.0e-4

    local max, min, abs, rad, cos, sin, floor = math.max, math.min, math.abs, math.rad, math.cos, math.sin, math.floor
    local random, randomseed = math.random, math.randomseed
    local getvalue, getoption, effect = obj.getvalue, obj.getoption, obj.effect
    local copybuffer, pixelshader = obj.copybuffer, obj.pixelshader
    local LAYER, TOTALTIME = obj.layer, obj.totaltime
    local ID, INDEX, NUM = obj.effect_id, obj.index, obj.num
    local dt = 1.0 / obj.framerate

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
        duration = duration * dt
        motion_cutoff = motion_cutoff * dt
        blink_duration = blink_duration * dt
        echo_interval = echo_interval * dt
    end

    if abs(duration) < eps then
        return
    end

    local motion, mask

    do
        local CACHE_LUT = "cache:a09d04ba-6a95-4940-b8f4-41f9d7483817-" .. ID

        if should_load_lut then
            local ok, e = pcall(function()
                if not copybuffer("cache:tmp", "object") then
                    error("Buffer copy operation failed")
                end

                local xform = {}
                copy_xform(xform, obj)

                if tint_source == 0 then
                    if not obj.load("image", tint_image) then
                        if not copybuffer("object", "cache:tmp") then
                            error("Buffer copy operation failed")
                        end
                        copy_xform(obj, xform)

                        error("Image file could not be loaded")
                    end
                elseif tint_source == 1 then
                    if not obj.load("layer", tint_layer, true) then
                        if not copybuffer("object", "cache:tmp") then
                            error("Buffer copy operation failed")
                        end
                        copy_xform(obj, xform)

                        error("Layer data could not be retrieved")
                    end
                end

                if not copybuffer(CACHE_LUT, "object") then
                    error("Buffer copy operation failed")
                end

                if not copybuffer("object", "cache:tmp") then
                    error("Buffer copy operation failed")
                end

                copy_xform(obj, xform)
            end)

            if not ok then
                print("@error", e)
                return
            end
        end

        local group

        do
            local regex = obj.module("Regex@${PROJECT_NAME}")

            group = function(mode)
                local t = {}
                local text = getvalue("テキスト", "テキスト")
                if text ~= nil then
                    text = text:gsub("\\\\", "\\"):gsub("\\n", "\n"):gsub("<.->", "")

                    local i, id, pattern
                    if mode == 1 then
                        i = -1
                        id = -1
                        pattern = [=[[^\s\v\x85\pZ]]=]
                    elseif mode == 2 then
                        i = 0
                        id = -2
                        pattern = [=[[\s\v\x85\pZ]]=]
                    else
                        i = 0
                        id = -3
                        pattern = "\\n"
                    end

                    for _, m in ipairs({ regex.search(id, text, pattern) }) do
                        if m[1] then
                            i = i + 1
                        end

                        if not m[2] then
                            t[#t + 1] = max(i, 0)
                        end
                    end
                else
                    for i = 1, NUM do
                        t[i] = i - 1
                    end
                end

                return t
            end
        end

        local function permute(n)
            randomseed(seed)

            local t = {}

            for i = 1, n do
                t[i] = i - 1
            end

            for i = n, 1, -1 do
                local j = random(1, i)
                t[i], t[j] = t[j], t[i]
            end

            return t
        end

        local get_time

        do
            local n = getoption("section_num") + 1

            get_time = function(i)
                return i < 0 and getvalue("time", 0.0, n + i) or getvalue("time", 0.0, i)
            end
        end

        local function interpolate(t)
            t = clamp(t, 0.0, 1.0)

            local st = getvalue(0, 0.0)
            local range = getvalue(0, 0.0, -1) - st

            return abs(range) > eps and (getvalue(0, TOTALTIME * t) - st) / range or t
        end

        local function progress(t, i, n)
            t = duration < 0.0 and t - TOTALTIME - duration or t
            local d = duration * (motion_overlap + (1.0 - motion_overlap) / n) * max(motion_softness, eps)

            if timing == 1 then
                i = duration < 0.0 and get_time(i - n - 1) - TOTALTIME - duration or get_time(i)
                i = i * (d + duration) / duration
            else
                i = abs(duration) * i / n
            end

            i = i - min(d, 0.0)

            local p = t / duration + (t - i) / d
            local w = 1.0 - interpolate(p)

            if duration < 0.0 then
                return (duration * i) / (duration + d) - t, -w
            else
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

                obj.ox = obj.ox + xform_position_x * w
                obj.oy = obj.oy + xform_position_y * w
                obj.oz = obj.oz + xform_position_z * w
                obj.alpha = lerp(obj.alpha, obj.alpha * xform_opacity, abs(w))
            end
        end

        local apply_lyric, apply_blink

        do
            local hash = obj.module("Hash@${PROJECT_NAME}")
            local hash4d = hash.hash4d

            local should_blink_opacity = blink_opacity_max - blink_opacity_min > eps
            local should_blink_scale = blink_scale_max - blink_scale_min > eps
            local should_edge_detect = blink_edge_threshold > -100.0 + eps

            apply_lyric = function(i, n, t, w)
                local hx, hy, hz, _ = hash4d(i, n, seed)

                local r = lerp(lyric_distance, lyric_distance * hx, lyric_jitter) * w
                local s = lerp(lyric_stretch, lyric_stretch * hy, lyric_jitter) * w

                local d = 0.0
                if lyric_shape == 0 then
                    d = 2.0 * math.pi / lyric_divisions
                elseif lyric_divisions ~= 1 then
                    d = lyric_sweep / (lyric_divisions - 1)
                end

                local angle = lyric_angle
                if lyric_selection == 0 then
                    angle = angle + d * floor(hz * lyric_divisions)
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

                if t < dt then
                    local p = 2.0 ^ lyric_peak
                    obj.sx = obj.sx * p
                    obj.sy = obj.sy * p
                elseif lyric_should_overshoot and t < 2.0 * dt then
                    local p = 2.0 ^ (-lyric_peak * 0.5)
                    obj.sx = obj.sx * p
                    obj.sy = obj.sy * p
                end
            end

            apply_blink = function(i, n, time)
                local hx, hy, hz, _ = hash4d(i, n, seed + 1, time * 1000.0)

                if should_blink_opacity then
                    obj.alpha = lerp(blink_opacity_min, blink_opacity_max, floor(hx * blink_steps) / (blink_steps - 1))
                end

                if should_blink_scale then
                    local r = floor(hy * blink_steps) / (blink_steps - 1)
                    local scale = lerp(blink_scale_min, blink_scale_max, r)
                    obj.sx = scale
                    obj.sy = scale
                    obj.sz = scale
                end

                if should_edge_detect and hz < 0.5 then
                    local ok, e = pcall(function()
                        if not copybuffer("cache:tmp", "object") then
                            error("Buffer copy operation failed")
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
                            error("Buffer copy operation failed")
                        end
                    end)

                    if not ok then
                        print("@error", e)
                        return
                    end
                end
            end
        end

        local apply_fx

        if effect_params ~= "" then
            local toml = obj.module("Toml@${PROJECT_NAME}")

            local KEY_FX = "99c01c07-d684-4b55-a6ca-7cd4bb9e390c-" .. ID

            local data
            if INDEX == 0 then
                data = { toml.parse("motion::effect", effect_params) }
                _G[KEY_FX] = data
            else
                data = _G[KEY_FX]
            end

            if type(data) ~= "table" then
                print("@warn", "Shared FX data table is missing or corrupted")
            end

            if INDEX == NUM - 1 then
                _G[KEY_FX] = nil
            end

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
                rand = obj.rand,
                rand1 = obj.rand1,
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
        end

        do
            local KEY_ORDER = "66d63ef0-5c82-46b4-8989-12fa433c66e9-" .. ID
            local KEY_GROUP = "a7d6bd98-24a1-480a-bf02-679a2c80c83d-" .. ID

            local i, n = INDEX, NUM

            if motion_based_on > 0 and getoption("multi_object") then
                local t
                if INDEX == 0 then
                    t = group(motion_based_on)
                    _G[KEY_GROUP] = t
                else
                    t = _G[KEY_GROUP]
                end

                if type(t) == "table" and #t == NUM then
                    i = t[i + 1]
                    n = t[n] + 1
                else
                    print("@warn", "Shared motion group table is missing or corrupted")
                end

                if INDEX == NUM - 1 then
                    _G[KEY_GROUP] = nil
                end
            elseif motion_based_on == -1 then
                i = 0
                n = 1
            end

            if motion_order == 1 then
                i = n - 1 - i
            elseif motion_order == 2 then
                local t
                if INDEX == 0 then
                    t = permute(n)
                    _G[KEY_ORDER] = t
                else
                    t = _G[KEY_ORDER]
                end

                if type(t) == "table" and #t == n then
                    i = t[i + 1]
                else
                    print("@warn", "Shared motion order table is missing or corrupted")
                end

                if INDEX == NUM - 1 then
                    _G[KEY_ORDER] = nil
                end
            end

            if timing == 1 then
                local sections = getoption("section_num")
                if sections < n then
                    print("@warn", (n - sections) .. " more sections are required to map all units")
                end

                duration = duration < 0.0 and get_time(-n - 1) - TOTALTIME or get_time(n)

                if abs(duration) < eps then
                    return
                end
            end

            motion = function(time, offset)
                local t, w = progress(time, i, n)

                apply_xform(w)

                if lyric_distance > eps then
                    apply_lyric(i, n, t, w)
                end

                if blink_duration > eps and t < blink_duration then
                    apply_blink(i, n, time)
                end

                if should_load_lut then
                    pixelshader("map", "object", { "object", CACHE_LUT }, { abs(w), offset }, "copy", "clamp")
                end

                if effect_params ~= "" then
                    apply_fx(w)
                end

                if t < motion_cutoff then
                    obj.alpha = eps
                end

                if should_highlight then
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

        do
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

    local time = obj.time

    if echo_count > 1 then
        local CACHE_IMAGE = "cache:cc2b9a9b-89d5-4481-b8f0-58ca17cd499f-" .. ID

        if not copybuffer(CACHE_IMAGE, "object") then
            print("@error", "Buffer copy operation failed")
            return
        end

        local xform = {}
        copy_xform(xform, obj)

        local i = -1
        obj.multiobject(echo_count, function()
            i = i + 1

            if not copybuffer("object", CACHE_IMAGE) then
                print("@error", "Buffer copy operation failed")
                return
            end

            copy_xform(obj, xform)

            local j = echo_composite == 0 and i or echo_count - i - 1
            local t = echo_interval * j
            local offset = j / (echo_count - 1)

            if motion_should_mask then
                mask(function()
                    motion(clamp(time + t, 0.0, TOTALTIME), offset)
                end)
            else
                motion(clamp(time + t, 0.0, TOTALTIME), offset)
            end

            obj.alpha = obj.alpha * (echo_decay ^ j)

            return t
        end)
    else
        if motion_should_mask then
            mask(function()
                motion(time, 0.0)
            end)
        else
            motion(time, 0.0)
        end
    end
end
