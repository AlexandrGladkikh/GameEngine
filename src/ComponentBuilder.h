#pragma once

#include <memory>
#include <string>

#include <rapidjson/document.h>

namespace engine {

class Component;

class ComponentBuilder {
public:
    static std::unique_ptr<Component> build(const std::string& type, rapidjson::Value& component);
};

}
