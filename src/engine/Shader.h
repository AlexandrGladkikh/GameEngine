#pragma once

#include "UtilGL.h"

#include <glm/glm.hpp>

#include <string>
#include <filesystem>
#include <memory>
#include <optional>

namespace engine {

class Shader {
public:
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
    void setUniform3mat(const std::string& name, const glm::mat3& value) const;
    void setUniform2mat(const std::string& name, const glm::mat2& value) const;
    void setUniform4vec(const std::string& name, const glm::vec4& value) const;
    void setUniform3vec(const std::string& name, const glm::vec3& value) const;
    void setUniform2vec(const std::string& name, const glm::vec2& value) const;
    void setUniform1vec(const std::string& name, const glm::vec1& value) const;
    void setUniform1f(const std::string& name, float value) const;
    void setUniform1d(const std::string& name, double value) const;
    void setUniform1i(const std::string& name, int value) const;
    void setUniform1ui(const std::string& name, uint32_t value) const;
    void setUniform1b(const std::string& name, bool value) const;

private:
    std::string m_name;

    GLuint m_program;

    std::vector<Uniform> m_uniforms{};
};

auto buildShader(const std::filesystem::path& path) -> std::optional<std::unique_ptr<Shader>>;

}