#pragma once

#include <rapidjson/document.h>

#include <memory>
#include <string>
#include <optional>
#include <vector>

namespace engine {

class Component;
class CameraComponent;
class MeshComponent;
class MaterialComponent;
class TransformComponent;
class FlipbookAnimationComponent;
class MouseEventFilterComponent;
class RenderScopeComponent;
class RenderPassComponent;
class LightSourceComponent;

class ComponentBuilder {
public:
    static auto componentTypes() -> const std::vector<std::string>&;
    static auto buildFromJson(const std::string& type, const rapidjson::Value& component) -> std::optional<std::unique_ptr<Component>>;
    static void saveToJson(const std::shared_ptr<Component>& component, rapidjson::Value& component_json, rapidjson::Document::AllocatorType& allocator);

    template<typename T>
    static auto buildEmptyComponent(const std::string& name, uint32_t owner_node, uint32_t owner_scene) -> std::optional<std::unique_ptr<T>>;
    static auto buildEmptyComponent(const std::string& type, const std::string& name, uint32_t owner_node, uint32_t owner_scene) -> std::optional<std::unique_ptr<Component>>;
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

template <>
std::optional<std::unique_ptr<MouseEventFilterComponent>> ComponentBuilder::buildEmptyComponent<MouseEventFilterComponent>(const std::string &name, uint32_t owner_node, uint32_t owner_scene);

template<>
std::optional<std::unique_ptr<RenderScopeComponent>> ComponentBuilder::buildEmptyComponent<RenderScopeComponent>(const std::string& name, uint32_t owner_node, uint32_t owner_scene);

template<>
std::optional<std::unique_ptr<RenderPassComponent>> ComponentBuilder::buildEmptyComponent<RenderPassComponent>(const std::string& name, uint32_t owner_node, uint32_t owner_scene);

template<>
std::optional<std::unique_ptr<LightSourceComponent>> ComponentBuilder::buildEmptyComponent<LightSourceComponent>(const std::string& name, uint32_t owner_node, uint32_t owner_scene);
}
