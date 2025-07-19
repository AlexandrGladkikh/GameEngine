#include "ComponentBuilder.h"

#include "CameraComponent.h"
#include "MaterialComponent.h"
#include "MeshComponent.h"

#include <optional>

namespace engine {

auto buildMaterialComponent(rapidjson::Value& componentData) -> std::optional<std::unique_ptr<MaterialComponent>>
{
    auto id = componentData["id"].GetUint();
    auto name = componentData["name"].GetString();
    auto owner = componentData["owner"].GetUint();

    auto shader_id = componentData["shader"].GetUint();
    auto texture_id = componentData["texture"].GetUint();

    auto component = std::make_unique<MaterialComponent>(id, name, owner, shader_id, texture_id);

    return component;
}

auto buildMeshComponent(rapidjson::Value& componentData) -> std::optional<std::unique_ptr<MeshComponent>>
{
        auto id = componentData["id"].GetUint();
        auto name = componentData["name"].GetString();
        auto owner = componentData["owner"].GetUint();
        auto meshId = componentData["mesh"].GetUint();

        auto component = std::make_unique<MeshComponent>(id, name, owner, meshId);

        return component;
}

auto buildCameraComponent(rapidjson::Value& componentData) -> std::optional<std::unique_ptr<CameraComponent>>
{
    auto id = componentData["id"].GetUint();
    auto name = componentData["name"].GetString();
    auto owner = componentData["owner"].GetUint();

    auto component = std::make_unique<CameraComponent>(id, name, owner);

    return component;
}

auto ComponentBuilder::build(const std::string& type, rapidjson::Value& component)
{
    if (type == "material") {
        return buildMaterialComponent(component);
    } else if (type == "mesh") {
        return buildMeshComponent(component);
    } else if (type == "camera") {
        return buildCameraComponent(component);
    }

    return std::nullopt;
}

}
