#include "SceneLoader.h"

#include "Scene.h"
#include "ResourcePackageStore.h"
#include "Context.h"
#include "ResourcePackage.h"

namespace engine {

auto SceneLoader::loadScene(const std::shared_ptr<Context>& context, const std::filesystem::path& path) -> std::optional<std::unique_ptr<Scene>>
{
    auto scene = buildScene(context, path);

    if (!scene.has_value()) {
        return std::nullopt;
    }

    auto& scene_value = scene.value();

    auto resources = scene_value->getResources();

    for (auto resource : resources) {
        auto path = context->resourcePackageStore->getResourcePackageInformation(resource);
        if (!path.has_value()) {
            continue;
        }

        auto resource_package = buildResourcePackage(path.value());
        if (!resource_package.has_value()) {
            continue;
        }

        loadResourcePackage(context, resource_package.value());
    }

    return scene;
}
}
