#include "SceneStore.h"

namespace engine {

auto SceneStore::get(const std::string& id) const
{
    auto it = m_scenes.find(id);
    if (it == m_scenes.end()) {
        return std::nullopt;
    }
    return it->second;
}

void SceneStore::add(const std::string& id, std::unique_ptr<Scene> scene)
{
    m_scenes.insert({id, std::move(scene)});
}

}