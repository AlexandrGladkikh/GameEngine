#pragma once

#include "Component.h"

#include <rapidjson/document.h>

namespace engine {

class MaterialComponent final : public Component {
public:
    MaterialComponent(uint32_t id, const std::string& name, uint32_t owner,
        uint32_t shader_id, uint32_t texture_id);

    auto shader() const -> uint32_t;
    auto texture() const -> uint32_t;

private:
    uint32_t m_shader_id;
    uint32_t m_texture_id;
};

}