--@Repeat

--require:${PROJECT_REQUIRES_AVIUTL2}
--information:Repeat@${SCRIPT_NAME} v${PROJECT_VERSION} by ${PROJECT_AUTHOR}
--label:${LABEL}

--group:Layout,true
--separator:Count
local layout_count_x = 1 --track@layout_count_x:Layout::Count::X,1,100,1,1
local layout_count_y = 1 --track@layout_count_y:Layout::Count::Y,1,100,1,1
--trackgroup@layout_count_x,layout_count_y:Group::Layout::Count
--separator:Padding
local layout_padding_x = 0.0 --track@layout_padding_x:Layout::Padding::X,-10000,10000,0,0.01
local layout_padding_y = 0.0 --track@layout_padding_y:Layout::Padding::Y,-10000,10000,0,0.01
--trackgroup@layout_padding_x,layout_padding_y:Group::Layout::Padding
--group:Position Offset,false
local position_offset_angle = 0.0 --track@position_offset_angle:Position Offset::Angle,-89,89,0,0.01
local position_offset_axis = 0.0 --track@position_offset_axis:Position Offset::Axis,-3600,3600,0,0.01
--group:Time Offset,false
local time_offset_interval = 0.0 --track@time_offset_interval:Time Offset::Interval,-100,100,0,0.001
local time_offset_orientation = 0 --select@time_offset_orientation:Time Offset::Orientation,Column=0,Row=1
local time_offset_order = 0 --select@time_offset_order:Time Offset::Order,Forward=0,Reverse=1,Random=2
--group:Additional Options,false
local unit = 0 --select@unit:Unit,Frames=0,Seconds=1
local seed = 0 --track@seed:Seed,-10000,10000,0,1
local should_highlight_order = false --check@should_highlight_order:Highlight Order,false

do
    --#include "utilities.lua"
    local utils = require("utilities")
    local copy_xform, stop = utils.copy_xform, utils.stop

    local CACHE_IMAGE = "cache:24a8ba19-70d6-4ceb-ad75-b793c122a10b-" .. obj.effect_id

    local max, floor, rad, random, randomseed = math.max, math.floor, math.rad, math.random, math.randomseed
    local copybuffer, pixelshader = obj.copybuffer, obj.pixelshader
    local TIME = obj.time

    layout_count_x = floor(layout_count_x)
    layout_count_y = floor(layout_count_y)

    position_offset_angle = rad(position_offset_angle)
    position_offset_axis = rad(position_offset_axis)

    time_offset_interval = unit == 0 and time_offset_interval / obj.framerate or time_offset_interval

    if seed >= 0 then
        randomseed(seed)
        seed = obj.layer + random(2147483647) + 1
    else
        seed = -seed
    end

    local size = layout_count_x * layout_count_y

    if size <= 1 then
        return
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

    local rx, ry = math.cos(position_offset_axis), math.sin(position_offset_axis)
    local s = math.tan(position_offset_angle)
    local c = s * rx * ry

    local w, h = obj.w, obj.h
    local dx, dy = w + layout_padding_x, h + layout_padding_y

    local ex, ey = dx * (1 - c), dx * s * rx * rx
    local fx, fy = -dy * s * ry * ry, dy * (1 + c)

    local ox = -0.5 * ((layout_count_x - 1) * ex + (layout_count_y - 1) * fx)
    local oy = -0.5 * ((layout_count_x - 1) * ey + (layout_count_y - 1) * fy)

    if not copybuffer(CACHE_IMAGE, "object") then
        stop("Failed to copy buffer")
        return
    end

    local xform = {}
    copy_xform(xform, obj)

    local order = (time_offset_order == 2) and permute(size) or nil

    local i = -1
    obj.multiobject(size, function()
        i = i + 1

        local j
        if time_offset_order == 0 then
            j = i
        elseif time_offset_order == 1 then
            j = size - i - 1
        elseif time_offset_order == 2 then
            ---@cast order integer[]
            j = order[i + 1]
        end

        if not copybuffer("object", CACHE_IMAGE) then
            stop("Failed to copy buffer")
            return
        end

        copy_xform(obj, xform)

        local row, col
        if time_offset_orientation == 0 then
            row = floor(j / layout_count_x)
            col = j - row * layout_count_x
        else
            col = floor(j / layout_count_y)
            row = j - col * layout_count_y
        end

        obj.ox = obj.ox + ox + ex * col + fx * row
        obj.oy = obj.oy + oy + ey * col + fy * row

        if should_highlight_order then
            pixelshader(
                "tint@Motion@${SCRIPT_NAME}",
                "object",
                "object",
                { 1.0, 0.0, 0.0, 1.0, 1.0 - i / max(size - 1, 1) }
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
