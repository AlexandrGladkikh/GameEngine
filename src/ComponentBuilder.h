#pragma once

#include <memory>
#include <string>
#include <optional>

#include <rapidjson/document.h>

namespace engine {

class Component;
class CameraComponent;
class MeshComponent;
class MaterialComponent;
class TransformComponent;

class ComponentBuilder {
public:
    static std::optional<std::unique_ptr<Component>> buildFromJson(const std::string& type, rapidjson::Value& component);
    static void saveToJson(const std::shared_ptr<Component>& component, rapidjson::Value& component_json, rapidjson::Document::AllocatorType& allocator);

    template<typename T>
    static std::optional<std::unique_ptr<Component>> buildEmptyComponent(const std::string& name, uint32_t owner_node, uint32_t owner_scene);
};

template<>
std::optional<std::unique_ptr<Component>> ComponentBuilder::buildEmptyComponent<CameraComponent>(const std::string& name, uint32_t owner_node, uint32_t owner_scene);

template<>
std::optional<std::unique_ptr<Component>> ComponentBuilder::buildEmptyComponent<MeshComponent>(const std::string& name, uint32_t owner_node, uint32_t owner_scene);

template<>
std::optional<std::unique_ptr<Component>> ComponentBuilder::buildEmptyComponent<MaterialComponent>(const std::string& name, uint32_t owner_node, uint32_t owner_scene);

template<>
std::optional<std::unique_ptr<Component>> ComponentBuilder::buildEmptyComponent<TransformComponent>(const std::string& name, uint32_t owner_node, uint32_t owner_scene);

}
