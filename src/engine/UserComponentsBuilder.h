#pragma once

#include <rapidjson/document.h>

#include <memory>
#include <optional>

namespace engine {

class Component;

class UserComponentsBuilder {
public:
    virtual ~UserComponentsBuilder() = default;
    virtual std::optional<std::unique_ptr<Component>> buildComponent(const std::string& type, rapidjson::Value& component) const = 0;
};

}
