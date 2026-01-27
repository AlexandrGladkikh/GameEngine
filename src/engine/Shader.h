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
    struct Uniform {
        std::string name;
        GLint location;
    };

    explicit Shader(const std::string& name, const std::string& vertexShader, const std::string& fragmentShader, const std::string& config);
    ~Shader();
    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;
    Shader(Shader&&) = delete;
    Shader& operator=(Shader&&) = delete;

    auto name() const -> std::string;

    void use() const;

    auto uniforms() const -> const std::vector<Uniform>&;

    void setUniform4mat(const std::string& name, const glm::mat4& value) const;
    void setUniform1i(const std::string& name, int value) const;

private:
    std::string m_name;

    GLuint m_program;

    std::vector<Uniform> m_uniforms{};
};

auto buildShader(const std::filesystem::path& path) -> std::optional<std::unique_ptr<Shader>>;

}