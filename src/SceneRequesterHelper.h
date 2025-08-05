#pragma once

#include "Scene.h"

#include <memory>
#include <optional>
#include <unordered_set>

namespace engine {

class SceneRequesterHelper {
public:
    template<typename T>
    static std::optional<std::shared_ptr<T>> getComponent(const std::shared_ptr<Scene>& scene, const std::unordered_set<uint32_t>& components)
    {
        for (auto component_id : components) {
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
    static bool hasComponent(const std::shared_ptr<Scene>& scene, const std::unordered_set<uint32_t>& components)
    {
        for (auto component_id : components) {
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
};

}
