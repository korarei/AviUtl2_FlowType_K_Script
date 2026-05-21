--@Echo

--require:${PROJECT_REQUIRES_AVIUTL2}
--information:Echo@${SCRIPT_NAME} v${PROJECT_VERSION} by ${PROJECT_AUTHOR}
--label:${LABEL}

local interval = 0.0 --track@interval:Interval,-100,100,-1,0.001
local count = 3 --track@count:Count,1,100,3,1
local decay = 50.0 --track@decay:Decay,0,100,50,0.01
local composite = 0 --select@composite:Composite=1,Above=0,Below=1
--group:Tint
local tint_source = 0 --select@tint_source:Tint::Source,Image=0,Layer=1
local tint_image = "" --file@tint_image:Tint::Image,""
local tint_layer = 0 --track@tint_layer:Tint::Layer,-100,100,0,1,---
--group:Additional Options,false
local unit = 0 --select@unit:Unit,Frames=0,Seconds=1
local layer_reference = 0 --select@layer_reference:Layer Reference,Absolute=0,Relative=1

if count < 2 then
    return
end

do
    --#include "utilities.lua"
    local utils = require("utilities")
    local copy_xform = utils.copy_xform

    local ID = obj.effect_id
    local CACHE_IMAGE = "cache:2261b3cd-be88-4ee0-b517-2314327fafe2-" .. ID
    local CACHE_LUT = "cache:2b17699d-4e70-47fa-ac79-ffc32f08f712-" .. ID

    local max = math.max
    local copybuffer, pixelshader = obj.copybuffer, obj.pixelshader
    local LAYER = obj.layer

    decay = decay * 0.01
    interval = unit == 0 and interval / obj.framerate or interval
    tint_layer = layer_reference == 0 and max(tint_layer, 0) or max(LAYER + tint_layer, 0)

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

    if should_load_lut then
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
    end

    if not copybuffer(CACHE_IMAGE, "object") then
        print("@error", "Buffer copy operation failed")
        return
    end

    local xform = {}
    copy_xform(xform, obj)

    local i = -1
    obj.multiobject(count, function()
        i = i + 1

        if not copybuffer("object", CACHE_IMAGE) then
            print("@error", "Buffer copy operation failed")
            return
        end

        copy_xform(obj, xform)

        local j = composite == 0 and i or count - i - 1

        if should_load_lut then
            pixelshader(
                "map@Motion@${SCRIPT_NAME}",
                "object",
                { "object", CACHE_LUT },
                { j / (count - 1), 0.5 },
                "copy",
                "clamp"
            )
        end

        obj.alpha = obj.alpha * (decay ^ j)

        return interval * j
    end)
end
