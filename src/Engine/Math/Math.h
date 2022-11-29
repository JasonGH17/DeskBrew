#pragma once

#include "api.h"

struct Vec2f {
    float x, y;
};
struct Vec2d {
    double x, y;
};
float vec2fDot(Vec2f v1, Vec2f v2);
double vec2dDot(Vec2d v1, Vec2d v2);

struct Vec3f {
    float x, y, z;
};
struct Vec3d {
    double x, y, z;
};
float vec3fDot(Vec3f v1, Vec3f v2);
Vec3f vec3fCross(Vec3f v1, Vec3f v2);
double vec3dDot(Vec3d v1, Vec3d v2);
Vec3d vec3dCross(Vec3d v1, Vec3d v2);

struct Vec4f {
    float x, y, z, w;
};
Vec4f vec4(float n);

struct Mat3 {
    Vec3f mat[3];
};
Mat3 mat3(float n);

struct Mat4 {
    Vec4f mat[4];
};
Mat4 mat4(float n);
Mat4 mat4(Mat3 &m);
