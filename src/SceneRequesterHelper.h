#pragma once

#include "Scene.h"
#include "Node.h"

#include <memory>
#include <optional>

namespace engine {

template<typename T>
std::optional<std::shared_ptr<T>> getComponent(const std::shared_ptr<Scene>& scene, const std::shared_ptr<Node>& node)
{
    for (auto component_id : node->components()) {
        auto component = scene->getComponent(component_id);
        if (!component.has_value()) {
            continue;
        }

        if (auto typed_component = std::dynamic_pointer_cast<T>(component.value())) {
            return typed_component;
        }
    }

    return std::nullopt;
}

template<typename T>
bool hasComponent(const std::shared_ptr<Scene>& scene, const std::shared_ptr<Node>& node)
{
    for (auto component_id : node->components()) {
        auto component = scene->getComponent(component_id);
        if (!component.has_value()) {
            continue;
        }

        if (auto typed_component = std::dynamic_pointer_cast<T>(component.value())) {
            return true;
        }
    }

    return false;
}

}
