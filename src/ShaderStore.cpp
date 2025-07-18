#include "ShaderStore.h"
#include "Shader.h"

namespace engine {

auto ShaderStore::get(const std::string& id) const -> std::optional<std::shared_ptr<Shader>>
{
    auto it = m_shaders.find(id);
    if (it == m_shaders.end()) {
        return std::nullopt;
    }
    return it->second;
}

void ShaderStore::add(const std::string& id, std::unique_ptr<Shader> shader)
{
    m_shaders[id] = std::move(shader);
}

}