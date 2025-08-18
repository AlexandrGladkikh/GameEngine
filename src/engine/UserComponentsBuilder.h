#pragma once

#include <rapidjson/document.h>

#include <memory>
#include <optional>
#include <vector>

namespace engine {

class Component;

class UserComponentsBuilder {
public:
    virtual ~UserComponentsBuilder() = default;
    virtual auto buildComponent(const std::string& type, rapidjson::Value& component) const -> std::optional<std::unique_ptr<Component>> = 0;
    virtual auto buildEmptyComponent(const std::string& type, const std::string& name, uint32_t owner_node, uint32_t owner_scene) -> std::optional<std::unique_ptr<Component>> = 0;
    virtual auto componentTypes() const -> const std::vector<std::string>& = 0;
};

}
