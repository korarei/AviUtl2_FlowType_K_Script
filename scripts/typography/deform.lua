--@Deform

--require:${PROJECT_REQUIRES_AVIUTL2}
--information:Deform@${SCRIPT_NAME} v${PROJECT_VERSION} by ${PROJECT_AUTHOR}
--label:${LABEL}

--group:Pivot Point,true
local pivot_x = 0.0 --track@pivot_x:Pivot::X,-100000,100000,0,0.01
local pivot_y = 0.0 --track@pivot_y:Pivot::Y,-100000,100000,0,0.01
--trackgroup@pivot_x,pivot_y:Group::Pivot Point
--group:Position,true
local position_x = 0.0 --track@position_x:Position::X,-100000,100000,0,0.01
local position_y = 0.0 --track@position_y:Position::Y,-100000,100000,0,0.01
--trackgroup@position_x,position_y:Group::Position
--group:Scale,true
local scale_x = 100.0 --track@scale_x:Scale::X,-10000,10000,100,0.01
local scale_y = 100.0 --track@scale_y:Scale::Y,-10000,10000,100,0.01
--trackgroup@scale_x,scale_y,scale_z:Group::Scale
--group:Skew,true
local skew_angle = 0.0 --track@skew_angle:Skew::Angle,-70,70,0,0.01
local skew_axis = 0 --track@skew_axis:Skew::Axis,-3600,3600,0,0.01
--group
local rotation = 0.0 --track@rotation:Rotation,-3600,3600,0,0.01
local opacity = 100.0 --track@opacity:Opacity,0,100,100,0.01
local sampling = 1 --select@sampling:Sampling=1,Nearest Neighbor=0,Bilinear=1

do
    local _ = obj.setanchor("position_x,position_y", 0, "line")

    local eps = 1.0e-4

    local rad, cos, sin, max, abs, ceil = math.rad, math.cos, math.sin, math.max, math.abs, math.ceil
    local setoption = obj.setoption
    local W, H = obj.w, obj.h

    pivot_x = pivot_x
    pivot_y = pivot_y
    scale_x = scale_x * 0.01
    scale_y = scale_y * 0.01
    skew_angle = rad(skew_angle)
    skew_axis = rad(skew_axis)
    rotation = rad(rotation)
    opacity = opacity * 0.01

    local transform

    do
        local rx, ry = cos(rotation), sin(rotation)
        local sx, sy = cos(skew_axis), sin(skew_axis)
        local skew = math.tan(skew_angle)

        transform = function(x, y)
            x, y = (x - pivot_x) * scale_x, (y - pivot_y) * scale_y
            local f = (x * sx - y * sy) * skew
            x, y = x + f * sy, y + f * sx
            x, y = x * rx - y * ry, x * ry + y * rx
            return x + position_x, y + position_y
        end
    end

    local cx, cy = W * 0.5, H * 0.5

    local x0, y0 = transform(-cx, -cy)
    local x1, y1 = transform(cx, -cy)
    local x2, y2 = transform(cx, cy)
    local x3, y3 = transform(-cx, cy)

    local w = ceil(max(abs(x0), abs(x1), abs(x2), abs(x3))) * 2
    local h = ceil(max(abs(y0), abs(y1), abs(y2), abs(y3))) * 2

    if w * h < eps then
        obj.clearbuffer("object", 0, 0)
        return
    end

    setoption("sampler", sampling == 0 and "dot" or "clamp")

    if not pcall(setoption, "drawtarget", "tempbuffer", w, h) then
        print("@error", "Buffer resize operation failed")
        return
    end

    local u0, v0 = 0, 0
    local u2, v2 = w, h

    obj.drawpoly(x0, y0, 0.0, x1, y1, 0.0, x2, y2, 0.0, x3, y3, 0.0, u0, v0, u2, v0, u2, v2, u0, v2, opacity)

    if not obj.copybuffer("object", "tempbuffer") then
        print("@error", "Buffer copy operation failed")
        return
    end

    if obj.getoption("multi_object") then
        obj.ox, obj.oy = transform(obj.ox, obj.oy)
    end
end
