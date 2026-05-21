local lerp, clamp, copy_xform

do
    local max, min = math.max, math.min

    lerp = function(a, b, t)
        return a + (b - a) * t
    end

    clamp = function(v, lower, upper)
        return max(lower, min(upper, v))
    end

    copy_xform = function(dst, src)
        dst.ox, dst.oy, dst.oz = src.ox, src.oy, src.oz
        dst.cx, dst.cy, dst.cz = src.cx, src.cy, src.cz
        dst.rx, dst.ry, dst.rz = src.rx, src.ry, src.rz
        dst.sx, dst.sy, dst.sz = src.sx, src.sy, src.sz
        dst.alpha = src.alpha
    end
end

if ... then
    return {
        lerp = lerp,
        clamp = clamp,
        copy_xform = copy_xform,
    }
end
