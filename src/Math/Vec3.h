#pragma once

namespace DeskBrew::Math {
    struct vec3f {
        float x, y, z;
    };

    inline float vec3fDot(vec3f v1, vec3f v2) {
        return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
    }

    inline vec3f vec3fCross(vec3f v1, vec3f v2) {
        return vec3f{
            v1.y*v2.z-v1.z*v2.y, 
            v1.z*v2.x-v1.x*v2.z, 
            v1.x*v2.y-v1.y*v2.x
        };
    }

    struct vec3d {
        double x, y, z;
    };

    inline double vec3dDot(vec3d v1, vec3d v2) {
        return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
    }

    inline vec3d vec3dCross(vec3d v1, vec3d v2) {
        return vec3d{
            v1.y*v2.z-v1.z*v2.y, 
            v1.z*v2.x-v1.x*v2.z, 
            v1.x*v2.y-v1.y*v2.x
        };
    }
}