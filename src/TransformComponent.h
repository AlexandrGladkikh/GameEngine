#pragma once

#include "Component.h"

#include <glm/glm.hpp>

namespace engine {

class TransformComponent : public Component {
public:
    TransformComponent(uint32_t id, const std::string& name, uint32_t owner_node, uint32_t owner_scene);
    ~TransformComponent() override = default;

    void init() override;

    void update(uint64_t dt) override;

    [[nodiscard]]
    bool isDirty() const override;
    void markDirty() override;
    void clearDirty() override;

    [[nodiscard]]
    std::string type() const override;

    glm::mat4 getModel();

    void setPosition(const glm::vec3& position);
    void setRotation(const glm::vec3& rotation);
    void setScale(const glm::vec3& scale);

    glm::vec3 getPosition() const;
    glm::vec3 getRotation() const;
    glm::vec3 getScale() const;

private:
    bool m_dirty;

    glm::mat4 m_model;

    glm::vec3 m_position;
    glm::vec3 m_rotation;
    glm::vec3 m_scale;
};

}
