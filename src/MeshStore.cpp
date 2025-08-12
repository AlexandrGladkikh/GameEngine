#include "MeshStore.h"

namespace engine {

void MeshData::bind() const
{
    glBindVertexArray(VAO);
}

void MeshData::unbind() const
{
    glBindVertexArray(0);
}

auto MeshStore::get(uint32_t id) const -> std::optional<std::shared_ptr<MeshData>>
{
    auto it = m_meshes.find(id);
    if (it == m_meshes.end()) {
        return std::nullopt;
    }
    return it->second;
}

auto MeshStore::get(const std::string& name) const -> std::optional<std::shared_ptr<MeshData>>
{
    for (const auto& [id, meshData] : m_meshes) {
        if (meshData->name == name) {
            return meshData;
        }
    }
    return std::nullopt;
}

auto MeshStore::getIdByName(const std::string &name) const -> std::optional<uint32_t>
{
    for (const auto& [id, meshData] : m_meshes) {
        if (meshData->name == name) {
            return id;
        }
    }
    return std::nullopt;
}

void MeshStore::remove(uint32_t id)
{
    m_meshes.erase(id);
}

void MeshStore::add(uint32_t id, const std::shared_ptr<MeshData>& meshData)
{
    m_meshes[id] = meshData;
}

}
