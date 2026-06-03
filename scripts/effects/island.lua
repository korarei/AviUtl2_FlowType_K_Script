--@Island

--require:${PROJECT_REQUIRES_AVIUTL2}
--information:Island@${SCRIPT_NAME} v${PROJECT_VERSION} by ${PROJECT_AUTHOR}
--label:${LABEL}

local threshold = 0.0 --track@threshold:Threshold,0,100,0,0.01
local index = -1 --track@index:Index,-1,100,-1,1
--group:Anchor,true
local anchor_target = 0 --select@anchor_target:Anchor::Target=1,Pivot Point=0,Position=1
local anchor_should_overwrite = false --checksection@anchor_should_overwrite:Anchor::Overwrite,false
--group:Sort,false
--separator:Order
local sort_order_primary_axis = 1 --select@sort_order_primary_axis:Sort::Order::Primary Axis=1,X=0,Y=1
local sort_order_x = 0 --select@sort_order_x:Sort::Order::X,Left to Right=0,Right to Left=1
local sort_order_y = 0 --select@sort_order_y:Sort::Order::Y,Top to Bottom=0,Bottom to Top=1
local sort_order_custom_order = {} --value@sort_order_custom_order:Sort::Order::Custom Order,{}
--separator:Blocks
local sort_blocks_x = 0 --track@sort_blocks_x:Sort::Blocks::X,0,50,0,1,---
local sort_blocks_y = 0 --track@sort_blocks_y:Sort::Blocks::Y,0,50,0,1,---
--group:Tint,false
local tint_source = 0 --select@tint_source:Tint::Source,Image=0,Layer=1
local tint_image = "" --file@tint_image:Tint::Image,""
local tint_layer = 0 --track@tint_layer:Tint::Layer,-100,100,0,1,---
local tint_order = 0 --select@tint_order:Tint::Order,Forward=0,Reverse=1,Random=2
--group:Time Offset,false
local time_offset_interval = 0.0 --track@time_offset_interval:Time Offset::Interval,-100,100,0,0.001
local time_offset_order = 0 --select@time_offset_order:Time Offset::Order,Forward=0,Reverse=1,Random=2
--group:Additional Options,false
local connectivity = 1 --select@connectivity:Connectivity=1,4-Connected=0,8-Connected=1
local unit = 0 --select@unit:Unit,Frames=0,Seconds=1
local layer_reference = 0 --select@layer_reference:Layer Reference,Absolute=0,Relative=1
local seed = 0 --track@seed:Seed,-10000,10000,0,1
local should_highlight_order = false --check@should_highlight_order:Highlight Order,false
--[[pixelshader@color_mask:
--#include <color_mask.hlsl>
]]

if obj.getoption("multi_object") then
    print("@error", "Disable Multi Object to use this script")
    return
end

do
    --#include "utilities.lua"
    local utils = require("utilities")
    local copy_xform = utils.copy_xform

    local island = obj.module("Island@${PROJECT_NAME}")
    local scan, fetch = island.scan, island.fetch

    local ID = obj.effect_id
    local CACHE_IMAGE = "cache:cf20d0bc-2ad4-4633-ab0f-2b349128aa13-" .. ID
    local CACHE_COLOR_MASK = "cache:dbd9e2b5-861a-4647-9bd7-cf8137f60c54-" .. ID
    local CACHE_ALPHA_MASK = "cache:9edded2f-c484-42b7-8bb8-032b0febed5e-" .. ID

    local max, random, randomseed = math.max, math.random, math.randomseed
    local copybuffer, clearbuffer, pixelshader = obj.copybuffer, obj.clearbuffer, obj.pixelshader
    local LAYER, TIME = obj.layer, obj.time

    threshold = math.floor(threshold * 2.55)

    tint_layer = layer_reference == 0 and max(tint_layer, 0) or max(LAYER + tint_layer, 0)
    time_offset_interval = unit == 0 and time_offset_interval / obj.framerate or time_offset_interval

    local should_use_custom_order = #sort_order_custom_order > 0

    local should_load_lut = false
    if tint_source == 0 then
        should_load_lut = tint_image ~= ""
    elseif tint_source == 1 then
        if tint_layer > 0 and tint_layer ~= LAYER then
            if not obj.getvalue("layer" .. tint_layer) then
                local frame = obj.getvalue("frame_s") + obj.frame
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

    local set_anchor

    do
        local x, y, sign
        if anchor_target == 0 then
            x, y, sign = "cx", "cy", -1.0
        elseif anchor_target == 1 then
            x, y, sign = "ox", "oy", 1.0
        end

        set_anchor = function(dx, dy)
            obj[x] = obj[x] + dx * sign
            obj[y] = obj[y] + dy * sign
        end
    end

    local apply_tint

    if should_load_lut then
        local CACHE_LUT = "cache:007fc33f-4843-45ca-8ef0-84edcea977c9-" .. ID

        local rand1 = obj.rand1

        local ok, e = pcall(function()
            if not copybuffer("cache:tmp", "object") then
                error("Buffer copy operation failed")
            end

            local xform = {}
            copy_xform(xform, obj)

            if tint_source == 0 then
                if not obj.load("image", tint_image) then
                    if not obj.copybuffer("object", "cache:tmp") then
                        error("Buffer copy operation failed")
                    end
                    copy_xform(obj, xform)

                    error("Image file could not be loaded")
                end
            elseif tint_source == 1 then
                if not obj.load("layer", tint_layer, true) then
                    if not obj.copybuffer("object", "cache:tmp") then
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

        apply_tint = function(i, n)
            local t
            if tint_order == 0 then
                t = i / max(n - 1, 1)
            elseif tint_order == 1 then
                t = 1.0 - i / max(n - 1, 1)
            else
                t = rand1(-seed, i)
            end

            pixelshader("map@Motion@${SCRIPT_NAME}", "object", { "object", CACHE_LUT }, { t, 0.5 }, "copy", "clamp")
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

    if not copybuffer(CACHE_COLOR_MASK, "object") then
        print("@error", "Buffer copy operation failed")
        return
    end

    if not copybuffer(CACHE_IMAGE, "object") then
        print("@error", "Buffer copy operation failed")
        return
    end

    local data, W, H = obj.getpixeldata(CACHE_COLOR_MASK)

    local n = scan(
        ID,
        threshold,
        connectivity,
        sort_order_primary_axis,
        sort_order_x,
        sort_order_y,
        sort_blocks_x,
        sort_blocks_y,
        data,
        W,
        H
    )

    if n == 0 then
        return
    end

    obj.putpixeldata(CACHE_COLOR_MASK, data, W, H)

    if index >= 0 and index < n then
        local x, y, w, h, dx, dy = fetch(ID, index)
        clearbuffer("object", w, h)
        pixelshader("color_mask", "object", { CACHE_IMAGE, CACHE_COLOR_MASK }, { x, y, index })

        if anchor_target == 0 then
            if anchor_should_overwrite then
                obj.cx = -dx
                obj.cy = -dy
            else
                obj.cx = obj.cx - dx
                obj.cy = obj.cy - dy
            end
        elseif anchor_target == 1 then
            if anchor_should_overwrite then
                obj.ox = dx
                obj.oy = dy
            else
                obj.ox = obj.ox + dx
                obj.oy = obj.oy + dy
            end
        end

        if should_load_lut then
            apply_tint(index, n)
        end

        return
    end

    local xform = {}
    copy_xform(xform, obj)
    if anchor_should_overwrite then
        if anchor_target == 0 then
            xform.cx, xform.cy = 0.0, 0.0
        elseif anchor_target == 1 then
            xform.ox, xform.oy = 0.0, 0.0
        end
    end

    if should_use_custom_order then
        n = math.min(n, #sort_order_custom_order + 1)
        clearbuffer(CACHE_ALPHA_MASK, W, H)
    end

    local order = time_offset_order == 2 and permute(n) or nil

    local i = -1
    obj.multiobject(n, function()
        i = i + 1

        local j
        if time_offset_order == 0 then
            j = i
        elseif time_offset_order == 1 then
            j = n - i - 1
        elseif time_offset_order == 2 then
            ---@cast order integer[]
            j = order[i + 1]
        end

        if should_use_custom_order then
            if i < n - 1 then
                j = sort_order_custom_order[j + 1]
                local x, y, w, h, dx, dy = fetch(ID, j)
                clearbuffer("object", w, h)
                pixelshader("color_mask", "object", { CACHE_IMAGE, CACHE_COLOR_MASK }, { x, y, j })
                pixelshader("color_mask", CACHE_ALPHA_MASK, { CACHE_IMAGE, CACHE_COLOR_MASK }, { 0, 0, j }, "draw")
                copy_xform(obj, xform)
                set_anchor(dx, dy)
            else
                if not copybuffer("object", CACHE_IMAGE) then
                    print("@error", "Buffer copy operation failed")
                    return
                end
                pixelshader("alpha_mask@Motion@${SCRIPT_NAME}", "object", CACHE_ALPHA_MASK, { 1.0 }, "mask")
                copy_xform(obj, xform)
            end
        else
            local x, y, w, h, dx, dy = fetch(ID, j)
            clearbuffer("object", w, h)
            pixelshader("color_mask", "object", { CACHE_IMAGE, CACHE_COLOR_MASK }, { x, y, j })
            copy_xform(obj, xform)
            set_anchor(dx, dy)
        end

        if should_load_lut then
            apply_tint(j, n)
        end

        if should_highlight_order then
            pixelshader(
                "tint@Motion@${SCRIPT_NAME}",
                "object",
                "object",
                { 1.0, 0.0, 0.0, 1.0, 1.0 - i / max(n - 1, 1) }
            )
        end

        local dt = time_offset_interval * i
        if TIME + dt < 0.0 then
            return -TIME
        else
            return dt
        end
    end)
end
