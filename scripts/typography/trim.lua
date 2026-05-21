--@Trim

--require:${PROJECT_REQUIRES_AVIUTL2}
--information:Trim@${SCRIPT_NAME} v${PROJECT_VERSION} by ${PROJECT_AUTHOR}
--label:${LABEL}

local threshold = 0.0 --track@threshold:Threshold,0,100,0,0.01
--group:Anchor,true
local anchor_target = 2 --select@anchor_target:Anchor::Target=2,None=0,Position=1,Pivot Point=2
local anchor_should_overwrite = false --checksection@anchor_should_overwrite:Anchor::Overwrite,false
--group:Padding,false
local padding_left = 0 --track@padding_left:Padding::Left,-1000,1000,0,1
local padding_right = 0 --track@padding_right:Padding::Right,-1000,1000,0,1
local padding_top = 0 --track@padding_top:Padding::Top,-1000,1000,0,1
local padding_bottom = 0 --track@padding_bottom:Padding::Bottom,-1000,1000,0,1
--[[pixelshader@blit
--#include <blit.hlsl>
]]

do
    local trim = obj.module("Trim@${PROJECT_NAME}")

    local max = math.max
    local W, H = obj.w, obj.h

    threshold = math.floor(threshold * 2.55)

    if not obj.copybuffer("cache:tmp", "object") then
        print("@error", "Buffer copy operation failed")
        return
    end

    local x, y, w, h = trim.trim(threshold, obj.getpixeldata("object"))

    x = x - padding_left
    y = y - padding_top
    w = max(w + padding_left + padding_right, 0)
    h = max(h + padding_top + padding_bottom, 0)

    if w * h < 1 then
        obj.effect("リサイズ", "拡大率", 0.0)
        return
    end

    obj.clearbuffer("object", w, h)
    obj.pixelshader("blit", "object", "cache:tmp", { x, y })

    local dx, dy = x + (w - W) * 0.5, y + (h - H) * 0.5

    if anchor_target == 1 then
        if anchor_should_overwrite then
            obj.ox = dx
            obj.oy = dy
        else
            obj.ox = obj.ox + dx
            obj.oy = obj.oy + dy
        end
    elseif anchor_target == 2 then
        if anchor_should_overwrite then
            obj.cx = -dx
            obj.cy = -dy
        else
            obj.cx = obj.cx - dx
            obj.cy = obj.cy - dy
        end
    end
end
