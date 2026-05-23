--@Transform

--require:${PROJECT_REQUIRES_AVIUTL2}
--information:Transform@${SCRIPT_NAME} v${PROJECT_VERSION} by ${PROJECT_AUTHOR}
--label:${LABEL}

--group:Pivot Point,true
local pivot_x = 0.0 --track@pivot_x:Pivot::X,-100000,100000,0,0.01
local pivot_y = 0.0 --track@pivot_y:Pivot::Y,-100000,100000,0,0.01
local pivot_z = 0.0 --track@pivot_z:Pivot::Z,-100000,100000,0,0.01
--trackgroup@pivot_x,pivot_y,pivot_z:Group::Pivot
--group:Position,true
local position_x = 0.0 --track@position_x:Position::X,-100000,100000,0,0.01
local position_y = 0.0 --track@position_y:Position::Y,-100000,100000,0,0.01
local position_z = 0.0 --track@position_z:Position::Z,-100000,100000,0,0.01
--trackgroup@position_x,position_y,position_z:Group::Position
--group:Rotation,true
local rotation_w = 0.0 --track@rotation_w:Rotation::W,-3600,3600,0,0.01
local rotation_x = 0.0 --track@rotation_x:Rotation::X,-3600,3600,0,0.01
local rotation_y = 0.0 --track@rotation_y:Rotation::Y,-3600,3600,0,0.01
local rotation_z = 0.0 --track@rotation_z:Rotation::Z,-3600,3600,0,0.01
--#define EULER XYZ Euler=5,XZY Euler=7,YXZ Euler=11,YZX Euler=15,ZXY Euler=19,ZYX Euler=21
local rotation_mode = 21 --select@rotation_mode:Rotation::Mode=21,Quaternion=0,Axis Angle=1,${EULER}
--trackgroup@rotation_x,rotation_y,rotation_z:Group::Rotation
--group:Scale,true
local scale_x = 100.0 --track@scale_x:Scale::X,-10000,10000,100,0.01
local scale_y = 100.0 --track@scale_y:Scale::Y,-10000,10000,100,0.01
local scale_z = 100.0 --track@scale_z:Scale::Z,-10000,10000,100,0.01
--trackgroup@scale_x,scale_y,scale_z:Group::Scale
--group:Target,true
local target_local_space = true --checksection@target_local_space:Target::Local Space,true
local target_world_space = false --checksection@target_world_space:Target::World Space,false
--group:Additional Options,false
local influence = 100.0 --track@influence:Influence,0,100,100,0.01

do
    local _ = obj.setanchor("position_x,position_y,position_z", 0, "line", "xyz")

    --#include "utilities.lua"
    local utils = require("utilities")
    local lerp = utils.lerp

    local vector = obj.module("Vector@${PROJECT_NAME}")
    local rotate = vector.rotate

    influence = influence * 0.01
    scale_x = scale_x * 0.01
    scale_y = scale_y * 0.01
    scale_z = scale_z * 0.01

    if target_world_space then
        local v = {
            (obj.ox - pivot_x * influence) * (1.0 + (scale_x - 1.0) * influence),
            (obj.oy - pivot_y * influence) * (1.0 + (scale_y - 1.0) * influence),
            (obj.oz - pivot_z * influence) * (1.0 + (scale_z - 1.0) * influence),
        }

        v = rotate(influence, rotation_mode, rotation_w, rotation_x, rotation_y, rotation_z, v)

        obj.ox = v[1]
        obj.oy = v[2]
        obj.oz = v[3]
    end

    if target_local_space then
        local rx, ry, rz = rotate(influence, rotation_mode, rotation_w, rotation_x, rotation_y, rotation_z)

        obj.cx = obj.cx + pivot_x * influence
        obj.cy = obj.cy + pivot_y * influence
        obj.cz = obj.cz + pivot_z * influence
        obj.rx = obj.rx + rx
        obj.ry = obj.ry + ry
        obj.rz = obj.rz + rz
        obj.sx = lerp(obj.sx, obj.sx * scale_x, influence)
        obj.sy = lerp(obj.sy, obj.sy * scale_y, influence)
        obj.sz = lerp(obj.sz, obj.sz * scale_z, influence)
    end

    obj.ox = obj.ox + position_x * influence
    obj.oy = obj.oy + position_y * influence
    obj.oz = obj.oz + position_z * influence
end
