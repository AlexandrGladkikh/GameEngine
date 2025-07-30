#include "TextureStore.h"
#include "Texture.h"

namespace engine {

auto TextureStore::get(uint32_t id) const -> std::optional<std::shared_ptr<Texture>>
{
    auto it = m_textures.find(id);
    if (it == m_textures.end()) {
            return std::nullopt;
    }
    return it->second;
}

void TextureStore::add(uint32_t id, std::unique_ptr<Texture> texture)
{
    m_textures[id] = std::move(texture);
}

void TextureStore::remove(uint32_t id)
{
    m_textures.erase(id);
}

}