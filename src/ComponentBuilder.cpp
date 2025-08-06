#include "ComponentBuilder.h"

#include "CameraComponent.h"
#include "MaterialComponent.h"
#include "MeshComponent.h"
#include "TransformComponent.h"
#include "Logger.h"
#include "Utils.h"

#include <optional>

namespace engine {

auto buildMaterialComponent(rapidjson::Value& componentData) -> std::optional<std::unique_ptr<MaterialComponent>>
{
    auto id = componentData["id"].GetUint();
    auto name = componentData["name"].GetString();
    auto owner_node = componentData["owner_node"].GetUint();
    auto owner_scene = componentData["owner_scene"].GetUint();

    auto component = std::make_unique<MaterialComponent>(id, name, owner_node, owner_scene);

    auto shader_id = componentData["shader"].GetUint();
    auto texture_id = componentData["texture"].GetUint();

    component->setShader(shader_id);
    component->setTexture(texture_id);

    return component;
}

auto saveMaterialComponent(const std::shared_ptr<MaterialComponent>& component, rapidjson::Value& component_json, rapidjson::Document::AllocatorType& allocator)
{
    rapidjson::Value value;
    value.SetString(component->name().c_str(), allocator);
    component_json.AddMember("type", "material", allocator);
    component_json.AddMember("id", component->id(), allocator);
    component_json.AddMember("name", value, allocator);
    component_json.AddMember("owner_node", component->ownerNode(), allocator);
    component_json.AddMember("owner_scene", component->ownerScene(), allocator);
    component_json.AddMember("shader", component->shader(), allocator);
    component_json.AddMember("texture", component->texture(), allocator);
}

auto buildMeshComponent(rapidjson::Value& componentData) -> std::optional<std::unique_ptr<MeshComponent>>
{
    auto id = componentData["id"].GetUint();
    auto name = componentData["name"].GetString();
    auto owner_node = componentData["owner_node"].GetUint();
    auto owner_scene = componentData["owner_scene"].GetUint();

    auto component = std::make_unique<MeshComponent>(id, name, owner_node, owner_scene);

    auto meshId = componentData["mesh"].GetUint();

    component->setMesh(meshId);

    return component;
}

auto saveMeshComponent(const std::shared_ptr<MeshComponent>& component, rapidjson::Value& component_json, rapidjson::Document::AllocatorType& allocator)
{
    rapidjson::Value value;
    value.SetString(component->name().c_str(), allocator);
    component_json.AddMember("type", "mesh", allocator);
    component_json.AddMember("id", component->id(), allocator);
    component_json.AddMember("name", value, allocator);
    component_json.AddMember("owner_node", component->ownerNode(), allocator);
    component_json.AddMember("owner_scene", component->ownerScene(), allocator);
    component_json.AddMember("mesh", component->meshId(), allocator);
}

auto buildCameraComponent(rapidjson::Value& componentData) -> std::optional<std::unique_ptr<CameraComponent>>
{
    auto id = componentData["id"].GetUint();
    auto name = componentData["name"].GetString();
    auto owner_node = componentData["owner_node"].GetUint();
    auto owner_scene = componentData["owner_scene"].GetUint();

    auto projection = componentData["projection"].GetObject();

    auto left = projection["left"].GetFloat();
    auto right = projection["right"].GetFloat();
    auto top = projection["top"].GetFloat();
    auto bottom = projection["bottom"].GetFloat();
    auto near = projection["near"].GetFloat();
    auto far = projection["far"].GetFloat();

    auto component = std::make_unique<CameraComponent>(id, name, owner_node, owner_scene);

    component->setOrtho(left, right, top, bottom, near, far);

    return component;
}

void saveCameraComponent(const std::shared_ptr<CameraComponent>& component, rapidjson::Value& component_json, rapidjson::Document::AllocatorType& allocator)
{
    rapidjson::Value value;
    value.SetString(component->name().c_str(), allocator);
    component_json.AddMember("type", "camera", allocator);
    component_json.AddMember("id", component->id(), allocator);
    component_json.AddMember("name", value, allocator);
    component_json.AddMember("owner_node", component->ownerNode(), allocator);
    component_json.AddMember("owner_scene", component->ownerScene(), allocator);
}

auto buildTransformComponent(rapidjson::Value& componentData) -> std::optional<std::unique_ptr<TransformComponent>>
{
    Logger::info(__FUNCTION__);

    auto id = componentData["id"].GetUint();
    auto name = componentData["name"].GetString();
    auto owner_node = componentData["owner_node"].GetUint();
    auto owner_scene = componentData["owner_scene"].GetUint();

    auto component = std::make_unique<TransformComponent>(id, name, owner_node, owner_scene);

    auto position_json = componentData["position"].GetArray();
    auto position = glm::vec3(position_json[0].GetFloat(), position_json[1].GetFloat(), position_json[2].GetFloat());
    component->setPosition(position);

    auto rotation_json = componentData["rotation"].GetArray();
    auto rotation = glm::vec3(rotation_json[0].GetFloat(), rotation_json[1].GetFloat(), rotation_json[2].GetFloat());
    component->setRotation(rotation);

    auto scale_json = componentData["scale"].GetArray();
    auto scale = glm::vec3(scale_json[0].GetFloat(), scale_json[1].GetFloat(), scale_json[2].GetFloat());
    component->setScale(scale);

    return component;
}

void saveTransformComponent(const std::shared_ptr<TransformComponent>& component, rapidjson::Value& component_json, rapidjson::Document::AllocatorType& allocator)
{
    rapidjson::Value value;
    value.SetString(component->name().c_str(), allocator);
    component_json.AddMember("type", "transform", allocator);
    component_json.AddMember("id", component->id(), allocator);
    component_json.AddMember("name", value, allocator);
    component_json.AddMember("owner_node", component->ownerNode(), allocator);
    component_json.AddMember("owner_scene", component->ownerScene(), allocator);

    rapidjson::Value position(rapidjson::kArrayType);
    position.PushBack(component->getPosition().x, allocator);
    position.PushBack(component->getPosition().y, allocator);
    position.PushBack(component->getPosition().z, allocator);
    component_json.AddMember("position", position, allocator);

    rapidjson::Value rotation(rapidjson::kArrayType);
    rotation.PushBack(component->getRotation().x, allocator);
    rotation.PushBack(component->getRotation().y, allocator);
    rotation.PushBack(component->getRotation().z, allocator);
    component_json.AddMember("rotation", rotation, allocator);

    rapidjson::Value scale(rapidjson::kArrayType);
    scale.PushBack(component->getScale().x, allocator);
    scale.PushBack(component->getScale().y, allocator);
    scale.PushBack(component->getScale().z, allocator);
    component_json.AddMember("scale", scale, allocator);
}

auto ComponentBuilder::buildFromJson(const std::string& type, rapidjson::Value& component) -> std::optional<std::unique_ptr<Component>>
{
    Logger::info(std::string(__FUNCTION__) + " component type: {}", type);

    if (type == "material") {
        return buildMaterialComponent(component);
    } else if (type == "mesh") {
        return buildMeshComponent(component);
    } else if (type == "camera") {
        return buildCameraComponent(component);
    } else if (type == "transform") {
        return buildTransformComponent(component);
    }

    return std::nullopt;
}

void ComponentBuilder::saveToJson(const std::shared_ptr<Component>& component, rapidjson::Value& component_json, rapidjson::Document::AllocatorType& allocator)
{
    if (component->type() == "material") {
        saveMaterialComponent(std::static_pointer_cast<MaterialComponent>(component), component_json, allocator);
    } else if (component->type() == "mesh") {
        saveMeshComponent(std::static_pointer_cast<MeshComponent>(component), component_json, allocator);
    } else if (component->type() == "camera") {
        saveCameraComponent(std::static_pointer_cast<CameraComponent>(component), component_json, allocator);
    } else if (component->type() == "transform") {
        saveTransformComponent(std::static_pointer_cast<TransformComponent>(component), component_json, allocator);
    }
}


template<>
std::optional<std::unique_ptr<Component>> ComponentBuilder::buildEmptyComponent<CameraComponent>(const std::string& name, uint32_t owner_node, uint32_t owner_scene)
{
    return std::make_unique<CameraComponent>(generateUniqueId(), name, owner_node, owner_scene);
}

template<>
std::optional<std::unique_ptr<Component>> ComponentBuilder::buildEmptyComponent<MeshComponent>(const std::string& name, uint32_t owner_node, uint32_t owner_scene)
{
    return std::make_unique<MeshComponent>(generateUniqueId(), name, owner_node, owner_scene);
}

template<>
std::optional<std::unique_ptr<Component>> ComponentBuilder::buildEmptyComponent<MaterialComponent>(const std::string& name, uint32_t owner_node, uint32_t owner_scene)
{
    return std::make_unique<MaterialComponent>(generateUniqueId(), name, owner_node, owner_scene);
}

template<>
std::optional<std::unique_ptr<Component>> ComponentBuilder::buildEmptyComponent<TransformComponent>(const std::string& name, uint32_t owner_node, uint32_t owner_scene)
{
    return std::make_unique<TransformComponent>(generateUniqueId(), name, owner_node, owner_scene);
}

}
