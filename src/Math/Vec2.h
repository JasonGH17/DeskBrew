#pragma once

namespace DeskBrew::Math {
    struct vec2f {
        float x, y;
    };

    inline float vec2fDot(vec2f v1, vec2f v2) {
        return v1.x * v2.x + v1.y * v2.y;
    }
    
    struct vec2d {
        double x, y;
    };

    inline double vec2dDot(vec2d v1, vec2d v2) {
        return v1.x * v2.x + v1.y * v2.y;
    }
} // namespace end
