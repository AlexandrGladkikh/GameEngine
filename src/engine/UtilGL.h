#pragma once

#include <glad/glad.h>

#include <string>

namespace engine {

struct Uniform {
    enum class Type {
        Float,
        Double,
        Int,
        UInt,
        Bool,
        Vec2,
        Vec3,
        Vec4,
        Mat2,
        Mat3,
        Mat4,
    };

    std::string name;
    GLint location;
    Type type;
};

}