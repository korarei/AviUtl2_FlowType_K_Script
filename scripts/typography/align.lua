--@Align

--require:${PROJECT_REQUIRES_AVIUTL2}
--information:Align@${SCRIPT_NAME} v${PROJECT_VERSION} by ${PROJECT_AUTHOR}
--label:${LABEL}

local horizontal = 0.0 --track@horizontal:Horizontal,-1000,1000,0,0.01
local vertical = 0.0 --track@vertical:Vertical,-1000,1000,0,0.01
local target = 1 --select@target:Target=1,Pivot Point=1,Position=2,Both=3
local should_overwrite = false --checksection@should_overwrite:Overwrite,false

do
    local cx, cy, ox, oy = 0.0, 0.0, 0.0, 0.0
    if not should_overwrite then
        cx, cy, ox, oy = obj.cx, obj.cy, obj.ox, obj.oy
    end

    local dx = horizontal * obj.w * 0.005
    local dy = vertical * obj.h * 0.005

    if bit.band(target, 1) ~= 0 then
        obj.cx, obj.cy = cx + dx, cy + dy
    end

    if bit.rshift(target, 1) ~= 0 then
        obj.ox, obj.oy = ox + dx, oy + dy
    end
end
