#include "SceneLoader.h"

#include "Scene.h"
#include "ResourcePackageStore.h"
#include "Context.h"
#include "ResourcePackage.h"
#include "Logger.h"

namespace engine {

auto SceneLoader::loadScene(const std::shared_ptr<Context>& context, const std::shared_ptr<SceneConfig>& scene_config) -> std::optional<std::unique_ptr<Scene>>
{
    Logger::info(__FUNCTION__);

    auto scene = buildScene(context, scene_config);

    if (!scene.has_value()) {
        return std::nullopt;
    }

    auto& scene_value = scene.value();

    auto resources_id = scene_value->getResources();

    for (auto resource_id : resources_id) {
        auto resource_package = context->resourcePackageStore->get(resource_id);
        if (!resource_package.has_value()) {
            auto path = context->resourcePackageStore->getResourcePackageInformation(resource_id);
            if (!path.has_value()) {
                continue;
            }

            resource_package = buildResourcePackage(path.value());
            if (!resource_package.has_value()) {
                continue;
            }

            context->resourcePackageStore->add(resource_id, resource_package.value());
        }

        if (resource_package.has_value()) {
            loadResourcePackage(context, resource_package.value());
        }
    }

    return scene;
}
}
