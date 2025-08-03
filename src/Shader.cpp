#include "Shader.h"
#include "FileSystem.h"
#include "Logger.h"

#include <glm/gtc/type_ptr.hpp>

namespace engine {

Shader::Shader(const std::string& vertexShader, const std::string& fragmentShader)
{
    Logger::info(__FUNCTION__);

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
    }

    const char* fragmentSource = fragmentShader.c_str();
    glShaderSource(fragment, 1, &fragmentSource, nullptr);
    glCompileShader(fragment);

    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment, 512, nullptr, infoLog);
        Logger::error("ERROR::SHADER::VERTEX::COMPILATION_FAILED, info: {}", infoLog);
    }

    m_program = glCreateProgram();
    glAttachShader(m_program, vertex);
    glAttachShader(m_program, fragment);
    glLinkProgram(m_program);

    glGetProgramiv(m_program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(m_program, 512, nullptr, infoLog);
        Logger::error("ERROR::SHADER::PROGRAM::LINKING_FAILED, info: {}", infoLog);
    }

    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

Shader::~Shader()
{
    glDeleteProgram(m_program);
}

void Shader::use() const
{
    glUseProgram(m_program);
}

void Shader::setUniform4mat(const std::string& name, const glm::mat4& value) const
{
    glUniformMatrix4fv(glGetUniformLocation(m_program, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::setUniform1i(const std::string& name, int value) const
{
    glUniform1i(glGetUniformLocation(m_program, name.c_str()), value);
}

auto buildShader(const std::filesystem::path& path) -> std::optional<std::unique_ptr<Shader>>
{
    Logger::info(__FUNCTION__);

    if (!FileSystem::exists(path) || !FileSystem::isDirectory(path)) {
        return std::nullopt;
    }

    auto shaderDirectory = FileSystem::directory(path);

    auto vertexShaderPath = shaderDirectory.path() / "vert.glsl";
    auto fragmentShaderPath = shaderDirectory.path() / "frag.glsl";

    if (!FileSystem::exists(vertexShaderPath) || !FileSystem::isFile(vertexShaderPath) ||
        !FileSystem::exists(fragmentShaderPath) || !FileSystem::isFile(fragmentShaderPath))
    {
        return std::nullopt;
    }

    auto vertexShaderSource = FileSystem::file(vertexShaderPath, std::ios::in).readText();
    auto fragmentShaderSource = FileSystem::file(fragmentShaderPath, std::ios::in).readText();
    return std::make_unique<Shader>(vertexShaderSource, fragmentShaderSource);
}

}