#pragma once

#include <memory>
#include <string>
#include <optional>

#include <rapidjson/document.h>

namespace engine {

class Component;

class ComponentBuilder {
public:
    static std::optional<std::unique_ptr<Component>> build(const std::string& type, rapidjson::Value& component);
};

}
