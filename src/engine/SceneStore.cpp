#include "SceneStore.h"
#include "Scene.h"

namespace engine {

auto SceneStore::get(uint32_t id) const -> std::optional<std::shared_ptr<Scene>>
{
    auto it = m_scenes.find(id);
    if (it == m_scenes.end()) {
        return std::nullopt;
    }
    return it->second;
}

void SceneStore::add(uint32_t id, std::unique_ptr<Scene> scene)
{
    m_scenes.insert({id, std::move(scene)});
}

void SceneStore::remove(uint32_t id)
{
    m_scenes.erase(id);
}

}