#include "Math.h"

float vec3fDot(Vec3f v1, Vec3f v2) {
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

Vec3f vec3fCross(Vec3f v1, Vec3f v2) {
    return Vec3f{
        v1.y*v2.z-v1.z*v2.y, 
        v1.z*v2.x-v1.x*v2.z, 
        v1.x*v2.y-v1.y*v2.x
    };
}

double vec3dDot(Vec3d v1, Vec3d v2) {
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

Vec3d vec3dCross(Vec3d v1, Vec3d v2) {
    return Vec3d{
        v1.y*v2.z-v1.z*v2.y, 
        v1.z*v2.x-v1.x*v2.z, 
        v1.x*v2.y-v1.y*v2.x
    };
}