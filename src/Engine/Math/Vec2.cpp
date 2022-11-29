#include "Math.h"

float vec2fDot(Vec2f v1, Vec2f v2) {
    return v1.x * v2.x + v1.y * v2.y;
}

double vec2dDot(Vec2d v1, Vec2d v2) {
    return v1.x * v2.x + v1.y * v2.y;
}