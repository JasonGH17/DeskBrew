#include "Math.h"

Mat3 mat3(float n) {
    Mat3 mat;
    for (int i = 0; i < 3; ++i) {
        mat.mat[i] = Vec3f{n, n, n};
    }
    return mat;
}