#include "TextureStore.h"
#include "Texture.h"

#include <algorithm>

namespace engine {

auto TextureStore::get(uint32_t id) const -> std::optional<std::shared_ptr<Texture>>
{
    auto it = m_textures.find(id);
    if (it == m_textures.end()) {
            return std::nullopt;
    }
    return it->second;
}

auto TextureStore::get(const std::string& name) const -> std::optional<std::shared_ptr<Texture>>
{
    auto it = std::ranges::find_if(m_textures, [&](const auto& texture) {
        return texture.second->name() == name;
    });
    if (it == m_textures.end()) {
        return std::nullopt;
    }
    return it->second;
}

auto TextureStore::getIdByName(const std::string& name) const -> std::optional<uint32_t>
{
    auto it = std::ranges::find_if(m_textures, [&](const auto& texture) {
        return texture.second->name() == name;
    });
    if (it == m_textures.end()) {
        return std::nullopt;
    }
    return it->first;
}

void TextureStore::add(uint32_t id, std::unique_ptr<Texture> texture)
{
    m_textures[id] = std::move(texture);
}

void TextureStore::remove(uint32_t id)
{
    m_textures.erase(id);
}

bool TextureStore::contains(uint32_t id) const
{
    return m_textures.contains(id);
}

}
