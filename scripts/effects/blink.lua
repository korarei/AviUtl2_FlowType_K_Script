--@Blink

--require:${PROJECT_REQUIRES_AVIUTL2}
--information:Blink@${SCRIPT_NAME} v${PROJECT_VERSION} by ${PROJECT_AUTHOR}
--label:${LABEL}

--#define BASED_ON Whole=-2,Objects=-1,Characters=0,Characters Excluding Spaces=1,Words=2,Lines=3
local based_on = 0 --select@based_on:Based On,${BASED_ON}
local duration = 0.0 --track@duration:Duration,-10000,10000,8,0.001,---
--group:Opacity,true
local opacity_steps = 2 --track@opacity_steps:Opacity::Steps,2,128,2,1
local opacity_min = 0.0 --track@opacity_min:Opacity::Minimum,0,100,0,0.01
local opacity_max = 100.0 --track@opacity_max:Opacity::Maximum,0,100,100,0.01
--group:Scale,false
local scale_steps = 2 --track@scale_steps:Scale::Steps,2,128,2,1
local scale_min = 100.0 --track@scale_min:Scale::Minimum,-10000,10000,100,0.01
local scale_max = 100.0 --track@scale_max:Scale::Maximum,-10000,10000,100,0.01
--group:Edge Detection,false
local edge_intensity = 100.0 --track@edge_intensity:Edge Detection::Intensity,0,1000,100,0.01
local edge_threshold = -100.0 --track@edge_threshold:Edge Detection::Threshold,-100,100,-100,0.01
--group:Characters,false
local characters_pool = "" --string@characters_pool:Characters::Pool,
local characters_font = "Noto Sans JP Black" --font@characters_font:Characters::Font,Noto Sans JP Black
local characters_scale = 100.0 --track@characters_scale:Characters::Scale,0,1000,100,0.01
--group:Color,false
local color_steps = 2 --track@color_steps:Color::Steps,2,128,2,1
local color_source = 0 --select@color_source:Color::Source,Image=0,Layer=1
local color_image = "" --file@color_image:Color::Image,""
local color_layer = 0 --track@color_layer:Color::Layer,-100,100,0,1,---
--group:Additional Options,false
local unit = 0 --select@unit:Unit,Frames=0,Seconds=1
local layer_reference = 0 --select@layer_reference:Layer Reference,Absolute=0,Relative=1
local seed = 0 --track@seed:Seed,-10000,10000,0,1

if obj.framerate == 0 then
    print("@error", "Framerate value must be greater than zero")
    return
end

do
    --#include "utilities.lua"
    local utils = require("utilities")
    local lerp, copy_xform, stop = utils.lerp, utils.copy_xform, utils.stop

    local buffer

    do
        buffer = require("string.buffer").new()
    end

    local utf8 = obj.module("UTF8@${PROJECT_NAME}")
    local hash = obj.module("Hash@${PROJECT_NAME}")
    local hash4d = hash.hash4d

    local eps = 1.0e-4

    local max, floor = math.max, math.floor
    local copybuffer, pixelshader = obj.copybuffer, obj.pixelshader
    local ID, INDEX, NUM, LAYER = obj.effect_id, obj.index, obj.num, obj.layer
    local FPS, TIME = obj.framerate, obj.time

    duration = unit == 0 and duration / FPS or duration

    opacity_min = opacity_min * 0.01
    opacity_max = opacity_max * 0.01

    scale_min = scale_min * 0.01
    scale_max = scale_max * 0.01

    color_layer = layer_reference == 0 and max(color_layer, 0) or max(LAYER + color_layer, 0)

    local should_blink_opacity = opacity_max - opacity_min > eps
    local should_blink_scale = scale_max - scale_min > eps
    local should_edge_detect = edge_threshold > -100.0 + eps

    local should_load_lut = false
    if color_source == 0 then
        should_load_lut = color_image ~= ""
    elseif color_source == 1 then
        if color_layer > 0 and color_layer ~= LAYER then
            if not obj.getvalue("layer" .. color_layer) then
                local frame = obj.getvalue("frame_s") + obj.frame
                print("@warn", "No object found in Layer " .. color_layer .. " at Frame " .. frame)
            else
                should_load_lut = true
            end
        end
    end

    if seed >= 0 then
        math.randomseed(seed)
        seed = LAYER + math.random(2147483647) + 1
    else
        seed = -seed
    end

    if math.abs(duration) < eps then
        print("@error", "Duration value is too small for this animation")
        return
    end

    local text = obj.getvalue(LAYER, "テキスト", "テキスト") --[[@as string | nil]]

    local i, n = INDEX, NUM

    if NUM > 1 then
        local content

        if based_on >= 0 then
            local KEY_COUNT = "0fd2dd98-70e4-4c71-a1a5-042eecbfdbe0-" .. ID

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

        if text ~= nil and based_on > 0 then
            local KEY_GROUP = "23ac879f-fa88-47fe-9047-98c62eb012d2-" .. ID

            local t
            if INDEX == 0 then
                local regex = obj.module("Regex@${PROJECT_NAME}")

                t = {}

                local j, id, pattern
                if based_on == 1 then
                    j = -1
                    id = -1
                    pattern = [=[[^\s\v\x85\pZ]]=]
                elseif based_on == 2 then
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
        elseif based_on == -2 then
            i = 0
            n = 1
        end
    end

    if (TIME - (duration < 0.0 and obj.totaltime or 0.0)) / duration < 1.0 then
        if characters_pool ~= "" then
            local alignment

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
                    characters_font,
                    obj.getvalue(LAYER, "テキスト", "サイズ") * characters_scale * 0.01,
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
                    characters_font,
                    max(obj.w, obj.h) * characters_scale * 0.01,
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

            local chars = utf8.split(characters_pool, true)

            local hx, _, _, _ = hash4d(i, n, seed, FPS * TIME * 100.0, 1, #chars)

            local xform = {}
            copy_xform(xform, obj)

            obj.load("text", chars[hx], 0.0, 0.0, alignment)

            copy_xform(obj, xform)
        end

        local hx, hy, hz, hw = hash4d(i, n, seed + 1, FPS * TIME * 100.0)

        if should_blink_opacity then
            obj.alpha = lerp(opacity_min, opacity_max, floor(hx * opacity_steps) / (opacity_steps - 1))
        end

        if should_blink_scale then
            local r = floor(hy * scale_steps) / (scale_steps - 1)
            local scale = lerp(scale_min, scale_max, r)
            obj.sx = scale
            obj.sy = scale
            obj.sz = scale
        end

        if should_edge_detect and hz < 0.5 then
            local ok, e = pcall(function()
                if not copybuffer("cache:tmp", "object") then
                    error("Failed to copy buffer")
                end

                obj.effect(
                    "エッジ抽出",
                    "強さ",
                    edge_intensity,
                    "しきい値",
                    edge_threshold,
                    "輝度エッジを抽出",
                    0,
                    "透明度エッジを抽出",
                    1
                )

                pixelshader("alpha_mask@Motion@${SCRIPT_NAME}", "cache:tmp", "object", { 0.0 }, "mask")

                if not copybuffer("object", "cache:tmp") then
                    error("Failed to copy buffer")
                end
            end)

            if not ok then
                stop(e)
                return
            end
        end

        if should_load_lut then
            local ok, e = pcall(function()
                local r = floor(hw * color_steps) / (color_steps - 1)

                if not copybuffer("cache:tmp", "object") then
                    error("Failed to copy buffer")
                end

                local xform = {}
                copy_xform(xform, obj)

                if color_source == 0 then
                    if not obj.load("image", color_image) then
                        if not copybuffer("object", "cache:tmp") then
                            error("Failed to copy buffer")
                        end

                        error("Failed to load image file")
                    end
                elseif color_source == 1 then
                    if not obj.load("layer", color_layer, true) then
                        if not copybuffer("object", "cache:tmp") then
                            error("Failed to copy buffer")
                        end

                        error("Failed to load layer data")
                    end
                end

                pixelshader(
                    "map@Motion@${SCRIPT_NAME}",
                    "cache:tmp",
                    { "cache:tmp", "object" },
                    { r, 0.5 },
                    "copy",
                    "clamp"
                )

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
    end
end
