#include "Shader.h"
#include "FileSystem.h"
#include "Logger.h"

#include <glm/gtc/type_ptr.hpp>

#include <rapidjson/document.h>

namespace engine {

Shader::Shader(const std::string& name, const std::string& vertexShader, const std::string& fragmentShader, const std::string& config) :
    m_name(name)
{
    Logger::debug(__FUNCTION__);

    GLuint vertex = 0;
    GLuint fragment = 0;

    vertex = glCreateShader(GL_VERTEX_SHADER);
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    const char* vertexSource = vertexShader.c_str();
    glShaderSource(vertex, 1, &vertexSource, nullptr);
    glCompileShader(vertex);

    GLint success = 0;
    GLchar infoLog[512];

    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex, 512, nullptr, infoLog);
        Logger::error("ERROR::SHADER::VERTEX::COMPILATION_FAILED, info: {}", infoLog);
        return;
    }

    const char* fragmentSource = fragmentShader.c_str();
    glShaderSource(fragment, 1, &fragmentSource, nullptr);
    glCompileShader(fragment);

    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment, 512, nullptr, infoLog);
        Logger::error("ERROR::SHADER::VERTEX::COMPILATION_FAILED, info: {}", infoLog);
        return;
    }

    m_program = glCreateProgram();
    glAttachShader(m_program, vertex);
    glAttachShader(m_program, fragment);
    glLinkProgram(m_program);

    glGetProgramiv(m_program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(m_program, 512, nullptr, infoLog);
        Logger::error("ERROR::SHADER::PROGRAM::LINKING_FAILED, info: {}", infoLog);
        return;
    }

    glDeleteShader(vertex);
    glDeleteShader(fragment);

    rapidjson::Document configJson = rapidjson::Document();
    configJson.Parse(config.c_str());

    if (configJson.HasMember("uniforms")) {
        auto uniforms = configJson["uniforms"].GetArray();
        for (const auto& uniform : uniforms) {
            if (uniform.IsObject() && uniform.HasMember("name") && uniform["name"].IsString() && uniform.HasMember("type") && uniform["type"].IsString()) {
                std::string name = uniform["name"].GetString();
                std::string type = uniform["type"].GetString();
                Uniform::Type uniformType;
                if (type == "Float") {
                    uniformType = Uniform::Type::Float;
                } else if (type == "Double") {
                    uniformType = Uniform::Type::Double;
                } else if (type == "Int") {
                    uniformType = Uniform::Type::Int;
                } else if (type == "UInt") {
                    uniformType = Uniform::Type::UInt;
                } else if (type == "Bool") {
                    uniformType = Uniform::Type::Bool;
                } else if (type == "Vec2") {
                    uniformType = Uniform::Type::Vec2;
                } else if (type == "Vec3") {
                    uniformType = Uniform::Type::Vec3;
                } else if (type == "Vec4") {
                    uniformType = Uniform::Type::Vec4;
                } else if (type == "Mat2") {
                    uniformType = Uniform::Type::Mat2;
                } else if (type == "Mat3") {
                    uniformType = Uniform::Type::Mat3;
                } else if (type == "Mat4") {
                    uniformType = Uniform::Type::Mat4;
                } else {
                    Logger::error("ERROR::SHADER::UNIFORM::INVALID, name: {}, type: {}", name, type);
                    continue;
                }

                m_uniforms.push_back({ name, glGetUniformLocation(m_program, name.c_str()), uniformType });
            }
        }
    }
}

Shader::~Shader()
{
    glDeleteProgram(m_program);
}

auto Shader::name() const -> std::string
{
    return m_name;
}

void Shader::use() const
{
    glUseProgram(m_program);
}

auto Shader::uniforms() const -> const std::vector<Uniform>&
{
    return m_uniforms;
}

void Shader::setUniform4mat(const std::string& name, const glm::mat4& value) const
{
    glUniformMatrix4fv(glGetUniformLocation(m_program, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::setUniform3mat(const std::string& name, const glm::mat3& value) const
{
    glUniformMatrix3fv(glGetUniformLocation(m_program, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::setUniform2mat(const std::string& name, const glm::mat2& value) const
{
    glUniformMatrix2fv(glGetUniformLocation(m_program, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::setUniform4vec(const std::string& name, const glm::vec4& value) const
{
    glUniform4fv(glGetUniformLocation(m_program, name.c_str()), 1, glm::value_ptr(value));
}

void Shader::setUniform3vec(const std::string& name, const glm::vec3& value) const
{
    glUniform3fv(glGetUniformLocation(m_program, name.c_str()), 1, glm::value_ptr(value));
}

void Shader::setUniform2vec(const std::string& name, const glm::vec2& value) const
{
    glUniform2fv(glGetUniformLocation(m_program, name.c_str()), 1, glm::value_ptr(value));
}

void Shader::setUniform1vec(const std::string& name, const glm::vec1& value) const
{
    glUniform1fv(glGetUniformLocation(m_program, name.c_str()), 1, glm::value_ptr(value));
}

void Shader::setUniform1f(const std::string& name, float value) const
{
    glUniform1f(glGetUniformLocation(m_program, name.c_str()), value);
}

void Shader::setUniform1d(const std::string& name, double value) const
{
    glUniform1d(glGetUniformLocation(m_program, name.c_str()), value);
}

void Shader::setUniform1ui(const std::string& name, uint32_t value) const
{
    glUniform1ui(glGetUniformLocation(m_program, name.c_str()), value);
}

void Shader::setUniform1b(const std::string& name, bool value) const
{
    glUniform1i(glGetUniformLocation(m_program, name.c_str()), value ? 1 : 0);
}

void Shader::setUniform1i(const std::string& name, int value) const
{
    glUniform1i(glGetUniformLocation(m_program, name.c_str()), value);
}

auto buildShader(const std::filesystem::path& path) -> std::optional<std::unique_ptr<Shader>>
{
    Logger::debug(__FUNCTION__);

    if (!FileSystem::exists(path) || !FileSystem::isDirectory(path)) {
        return std::nullopt;
    }

    auto shaderDirectory = FileSystem::directory(path);

    auto vertexShaderPath = shaderDirectory.path() / "vert.glsl";
    auto fragmentShaderPath = shaderDirectory.path() / "frag.glsl";

    auto configPath = shaderDirectory.path() / "config.json";

    if (!FileSystem::exists(vertexShaderPath) || !FileSystem::isFile(vertexShaderPath) ||
        !FileSystem::exists(fragmentShaderPath) || !FileSystem::isFile(fragmentShaderPath) ||
        !FileSystem::exists(configPath) || !FileSystem::isFile(configPath))
    {
        return std::nullopt;
    }

    auto vertexShaderSource = FileSystem::file(vertexShaderPath, std::ios::in).readText();
    auto fragmentShaderSource = FileSystem::file(fragmentShaderPath, std::ios::in).readText();
    auto configSource = FileSystem::file(configPath, std::ios::in).readText();

    return std::make_unique<Shader>(path.stem().string(), vertexShaderSource, fragmentShaderSource, configSource);
}

}