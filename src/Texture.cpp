#include "Texture.h"
#include "FileSystem.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

namespace engine {

Texture::Texture()
{
    glGenTextures(1, &m_texture);
}

Texture::Texture(void* data, GLsizei width, GLsizei height)
{
    glGenTextures(1, &m_texture);
    glBindTexture(GL_TEXTURE_2D, m_texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);
}

Texture::~Texture()
{
    glDeleteTextures(1, &m_texture);
}

void Texture::bind() const
{
    glBindTexture(GL_TEXTURE_2D, m_texture);
}
void Texture::unbind() const
{
    glBindTexture(GL_TEXTURE_2D, 0);
}

auto buildTexture(const std::filesystem::path& path) -> std::optional<std::unique_ptr<Texture>>
{
    auto file = FileSystem::file(path, std::ios::in | std::ios::binary);
    auto data = file.readBinary();
    if (data.empty()) {
        return std::nullopt;
    }

    int widthTex, heightTex, nrChannels;
    unsigned char* image = stbi_load_from_memory(data.data(), data.size(), &widthTex, &heightTex, &nrChannels, 0);

    return std::make_unique<Texture>(image, widthTex, heightTex);
}

}