#pragma once

#include <glad/glad.h>

#include <filesystem>
#include <memory>
#include <optional>
#include <string>

namespace engine
{

class Texture final {
public:
    explicit Texture(const std::string& name);
    explicit Texture(const std::string& name, void* data, GLsizei width, GLsizei height, GLint channels);
    ~Texture();
    Texture(const Texture&) = delete;
    Texture(Texture&&) = delete;
    Texture& operator=(const Texture&) = delete;
    Texture& operator=(Texture&&) = delete;

    std::string name() const;

    void bind() const;
    void unbind() const;

    GLuint width() const;
    GLuint height() const;

private:
    std::string m_name;

    GLuint m_texture = 0;

    GLuint m_width = 0;
    GLuint m_height = 0;
};

auto buildTexture(const std::filesystem::path& path) -> std::optional<std::unique_ptr<Texture>>;

}
