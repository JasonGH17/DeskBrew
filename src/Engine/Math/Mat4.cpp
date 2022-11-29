#include "Math.h"

Mat4 mat4(float n) {
    Mat4 mat;
    for (int i = 0; i < 4; ++i) {
        mat.mat[i] = vec4(n);
    }
    return mat;
}

Mat4 mat4(Mat3 &m) {
    Mat4 mat;
    for (int i = 0; i < 3; ++i) {
        Vec3f vec = m.mat[i];
        mat.mat[i] = Vec4f{vec.x, vec.y, vec.z, 0};
    }
    return mat;
}

