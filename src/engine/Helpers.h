#pragma once

#include "Component.h"
#include "Node.h"
#include "Scene.h"

#include <memory>
#include <string>

namespace engine {

template <typename T>
std::shared_ptr<T> getComponentByName(const Component* component, const std::string& name)
{
    auto node = component->getNode();
    if (!node.has_value()) {
        return nullptr;
    }

    auto scene = node.value()->getScene();
    if (!scene.has_value()) {
        return nullptr;
    }

    auto found_component = scene.value()->getComponent(name);
    if (!found_component) {
        return nullptr;
    }

    auto found_component_value = std::dynamic_pointer_cast<T>(found_component.value());
    if (!found_component_value) {
        return nullptr;
    }

    return found_component_value;
}

template <typename T>
std::shared_ptr<T> getComponentByName(const std::shared_ptr<Component>& component, const std::string& name)
{
    return getComponentByName<T>(component.get(), name);
}

template <typename T>
std::shared_ptr<T> getComponentById(const std::shared_ptr<Component>& component, uint32_t id)
{
    auto node = component->getNode();
    if (!node.has_value()) {
        return nullptr;
    }

    auto scene = node.value()->getScene();
    if (!scene.has_value()) {
        return nullptr;
    }

    auto found_component = scene.value()->getComponent(id);
    if (!found_component) {
        return nullptr;
    }

    auto found_component_value = std::dynamic_pointer_cast<T>(found_component.value());
    if (!found_component_value) {
        return nullptr;
    }

    return found_component_value;
}

}