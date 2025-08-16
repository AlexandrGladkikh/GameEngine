#pragma once

#include "Component.h"

namespace engine {

class MaterialComponent final : public Component {
public:
    explicit MaterialComponent(uint32_t id, const std::string& name, uint32_t owner_node, uint32_t owner_scene);

    void init() override;

    void update(uint64_t dt) override;

    [[nodiscard]]
    bool isDirty() const override;
    void markDirty() override;
    void clearDirty() override;

    std::string type() const override;

    void setShader(uint32_t shader_id);
    void setShader(const std::string& shader_name);
    void setTexture(uint32_t texture_id);
    void setTexture(const std::string& texture_name);

    auto shaderId() const -> uint32_t;
    auto textureId() const -> uint32_t;

    auto shaderName() const -> std::string;
    auto textureName() const -> std::string;

    auto textureSize() const -> std::pair<uint32_t, uint32_t>;

private:
    uint32_t m_shader_id = 0;
    uint32_t m_texture_id = 0;

    bool m_dirty = true;
};

}