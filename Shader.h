#pragma once

#include <glad/glad.h>

class Shader {
public:
    GLuint program;

    Shader(const char* vertexShader, const char* fragmentShader);

    void use();
};
