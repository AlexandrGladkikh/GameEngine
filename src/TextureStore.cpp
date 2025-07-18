#include "TextureStore.h"
#include "Texture.h"

namespace engine {

auto TextureStore::get(const std::string& id) const -> std::optional<std::shared_ptr<Texture>>
{
    auto it = m_textures.find(id);
    if (it == m_textures.end()) {
            return std::nullopt;
    }
    return it->second;
}

void TextureStore::add(const std::string& id, std::unique_ptr<Texture> texture)
{
    m_textures[id] = std::move(texture);
}

}