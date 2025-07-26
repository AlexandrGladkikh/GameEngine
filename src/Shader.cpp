#include "Shader.h"
#include <FileSystem.h>

namespace engine {

Shader::Shader(const std::string& vertexShader, const std::string& fragmentShader)
{
    GLuint vertex = 0;
    GLuint fragment = 0;

    vertex = glCreateShader(GL_VERTEX_SHADER);
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    const char* vertexSource = vertexShader.c_str();
    glShaderSource(vertex, 1, &vertexSource, nullptr);
    glCompileShader(vertex);
    const char* fragmentSource = fragmentShader.c_str();
    glShaderSource(fragment, 1, &fragmentSource, nullptr);
    glCompileShader(fragment);

    m_program = glCreateProgram();
    glAttachShader(m_program, vertex);
    glAttachShader(m_program, fragment);
    glLinkProgram(m_program);

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

auto buildShader(const std::filesystem::path& path) -> std::optional<std::unique_ptr<Shader>>
{
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