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
class FlipbookAnimationComponent;

class ComponentBuilder {
public:
    static std::optional<std::unique_ptr<Component>> buildFromJson(const std::string& type, rapidjson::Value& component);
    static void saveToJson(const std::shared_ptr<Component>& component, rapidjson::Value& component_json, rapidjson::Document::AllocatorType& allocator);

    template<typename T>
    static std::optional<std::unique_ptr<T>> buildEmptyComponent(const std::string& name, uint32_t owner_node, uint32_t owner_scene);
};

template<>
std::optional<std::unique_ptr<CameraComponent>> ComponentBuilder::buildEmptyComponent<CameraComponent>(const std::string& name, uint32_t owner_node, uint32_t owner_scene);

template<>
std::optional<std::unique_ptr<MeshComponent>> ComponentBuilder::buildEmptyComponent<MeshComponent>(const std::string& name, uint32_t owner_node, uint32_t owner_scene);

template<>
std::optional<std::unique_ptr<MaterialComponent>> ComponentBuilder::buildEmptyComponent<MaterialComponent>(const std::string& name, uint32_t owner_node, uint32_t owner_scene);

template<>
std::optional<std::unique_ptr<TransformComponent>> ComponentBuilder::buildEmptyComponent<TransformComponent>(const std::string& name, uint32_t owner_node, uint32_t owner_scene);

template<>
std::optional<std::unique_ptr<FlipbookAnimationComponent>> ComponentBuilder::buildEmptyComponent<FlipbookAnimationComponent>(const std::string& name, uint32_t owner_node, uint32_t owner_scene);

}
