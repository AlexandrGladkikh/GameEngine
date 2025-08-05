#pragma once

#include "Component.h"

namespace engine {

class MaterialComponent final : public Component {
public:
    explicit MaterialComponent(uint32_t id, const std::string& name, uint32_t owner_node, uint32_t owner_scene,
        uint32_t shader_id, uint32_t texture_id);

    void init() override;

    void update(uint64_t dt) override;

    [[nodiscard]]
    bool isDirty() const override;
    void markDirty() override;
    void clearDirty() override;

    std::string type() const override;

    auto shader() const -> uint32_t;
    auto texture() const -> uint32_t;

private:
    uint32_t m_shader_id;
    uint32_t m_texture_id;

    bool m_dirty;
};

}