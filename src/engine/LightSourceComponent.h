#pragma once

#include "Component.h"

#include <glm/glm.hpp>

namespace engine {

class LightSourceComponent final : public Component {
public:
    explicit LightSourceComponent(uint32_t id, const std::string& name, uint32_t owner_node, uint32_t owner_scene);
    ~LightSourceComponent() = default;

    void init() override;
    void update(uint64_t dt) override;

    [[nodiscard]]
    bool isDirty() const override;
    void markDirty() override;
    void clearDirty() override;

    auto type() const -> std::string override;

    auto clone(uint32_t owner_node_id) const -> std::unique_ptr<Component> override;

    void setColor(const glm::vec3& color);
    void setIntensity(float intensity);

    auto color() const -> glm::vec3;
    auto intensity() const -> float;

private:
    glm::vec3 m_color;
    float m_intensity;
};

}