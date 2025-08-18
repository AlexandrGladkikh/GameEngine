#include "ShaderStore.h"
#include "Shader.h"

namespace engine {

auto ShaderStore::get(uint32_t id) const -> std::optional<std::shared_ptr<Shader>>
{
    auto it = m_shaders.find(id);
    if (it == m_shaders.end()) {
        return std::nullopt;
    }
    return it->second;
}

auto ShaderStore::get(const std::string& name) const -> std::optional<std::shared_ptr<Shader>>
{
    auto it = std::ranges::find_if(m_shaders, [&](const auto& shader) {
        return shader.second->name() == name;
    });
    if (it == m_shaders.end()) {
        return std::nullopt;
    }
    return it->second;
}

auto ShaderStore::getIdByName(const std::string& name) const -> std::optional<uint32_t>
{
    auto it = std::ranges::find_if(m_shaders, [&](const auto& shader) {
        return shader.second->name() == name;
    });
    if (it == m_shaders.end()) {
        return std::nullopt;
    } else {
        return it->first;
    }
}

void ShaderStore::add(uint32_t id, std::unique_ptr<Shader> shader)
{
    m_shaders[id] = std::move(shader);
}

void ShaderStore::remove(uint32_t id)
{
    m_shaders.erase(id);
}

bool ShaderStore::contains(uint32_t id) const
{
    return m_shaders.contains(id);
}

}