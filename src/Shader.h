#pragma once

#include <glad/glad.h>

#include <glm/glm.hpp>

#include <string>
#include <filesystem>
#include <memory>
#include <optional>

namespace engine {

class Shader {
public:
    Shader(const std::string& vertexShader, const std::string& fragmentShader);
    ~Shader();
    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;
    Shader(Shader&&) = delete;
    Shader& operator=(Shader&&) = delete;

    void use() const;

    void setUniform4mat(const std::string& name, const glm::mat4& value) const;
    void setUniform1i(const std::string& name, int value) const;

private:
    GLuint m_program;
};

auto buildShader(const std::filesystem::path& path) -> std::optional<std::unique_ptr<Shader>>;

}