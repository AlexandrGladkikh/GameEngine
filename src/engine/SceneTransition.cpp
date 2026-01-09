#include "SceneTransition.h"
#include "Context.h"
#include "SceneStore.h"
#include "Scene.h"
#include "SceneLoader.h"
#include "ResourcePackageStore.h"
#include "MeshStore.h"
#include "ShaderStore.h"
#include "TextureStore.h"
#include "Logger.h"
#include "Component.h"
#include "ResourcePackage.h"
#include "SceneConfig.h"

#include <algorithm>
#include <ranges>

namespace engine {

SceneTransition::SceneTransition(const std::shared_ptr<Context>& context) :
        m_context(context)
{
    
}

bool SceneTransition::transition(const std::unordered_map<uint32_t, std::shared_ptr<SceneConfig>>& scene_infos, uint32_t scene_id_from, uint32_t scene_id_to)
{
    Logger::info(__FUNCTION__);

    auto scene_from = m_context->sceneStore->get(scene_id_from);
    std::vector<uint32_t> scene_from_resources;

    if (scene_from.has_value() && scene_from.value()->isActive()) {
        Logger::info("Scene {} disable", scene_id_from);

        scene_from.value()->setActive(false);

        scene_from_resources = scene_from.value()->getResources();

        scene_from.value().reset();
        m_context->sceneStore->remove(scene_id_from);
    }

    auto scene_info_to = scene_infos.find(scene_id_to);
    if (scene_info_to == scene_infos.end()) {
        return false;
    }

    auto scene_to = SceneLoader::loadScene(m_context, scene_info_to->second);
    if (!scene_to.has_value()) {
        return false;
    }

    const auto& scene_to_resources = scene_to.value()->getResources();
    for (auto resource_from : scene_from_resources) {
        if (std::find(scene_to_resources.begin(), scene_to_resources.end(), resource_from) == scene_to_resources.end()) {
            auto resource_package = m_context->resourcePackageStore->get(resource_from);
            if (!resource_package.has_value()) {
                continue;
            }

            for (auto& resource_info: resource_package.value()->meshes) {
                m_context->meshStore->remove(resource_info.id);
            }
            for (auto& resource_info: resource_package.value()->shaders) {
                m_context->shaderStore->remove(resource_info.id);
            }
            for (auto& resource_info: resource_package.value()->textures) {
                m_context->textureStore->remove(resource_info.id);
            }
        }
    }

    scene_to.value()->setActive(true);

    Logger::info("Scene {} enable", scene_id_to);
    uint32_t scene_id = scene_to.value()->id();
    m_context->sceneStore->add(scene_id, std::move(scene_to.value()));

    for (auto& component : m_context->sceneStore->get(scene_id).value()->getComponents() | std::views::values) {
        component->init();
    }

    return true;
}

}
