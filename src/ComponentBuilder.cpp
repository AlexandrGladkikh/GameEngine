#include "ComponentBuilder.h"

#include "MaterialComponent.h"

namespace engine {

auto buildMaterialComponent(rapidjson::Value& component) -> std::shared_ptr<MaterialComponent>;

auto ComponentBuilder::build(const std::string& type, rapidjson::Value& component)
{
    return nullptr;
}

}
