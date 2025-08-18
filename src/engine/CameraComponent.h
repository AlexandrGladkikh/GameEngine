#pragma once

#include "Component.h"

#include <glad/glad.h>
#include <glm/glm.hpp>

namespace engine {

class CameraComponent final : public Component {
public:
    struct Ortho {
        GLfloat left, right, top, bottom, near, far;
    };

    explicit CameraComponent(uint32_t id, const std::string& name, uint32_t owner_node, uint32_t owner_scene);
    ~CameraComponent() override = default;

    void init() override;

    void update(uint64_t dt) override;

    [[nodiscard]]
    bool isDirty() const override;
    void markDirty() override;
    void clearDirty() override;

    std::string type() const override;

    void setOrtho(GLfloat left, GLfloat right, GLfloat top, GLfloat bottom, GLfloat near, GLfloat far);
    void setOrtho(const Ortho& ortho);
    auto getOrtho() const -> Ortho;

    auto getView() const -> glm::mat4;
    auto getProjection() const -> glm::mat4;

    void setPosition(const glm::vec3& position);
    auto getPosition() const -> glm::vec3;

    void setYaw(GLfloat yaw);
    void setPitch(GLfloat pitch);

    auto getYaw() const -> GLfloat;
    auto getPitch() const -> GLfloat;

private:
    void updateView();

    Ortho m_ortho;

    glm::vec3 m_position;
    glm::vec3 m_front;
    glm::vec3 m_up;
    glm::vec3 m_right;
    glm::vec3 m_world_up;

    GLfloat m_yaw;
    GLfloat m_pitch;

    GLfloat m_fov;
    GLfloat m_near;
    GLfloat m_far;

    glm::mat4 m_view;

    glm::mat4 m_projection;

    bool m_dirty;

    constexpr static GLfloat DEFAULT_FOV = 45.0f;
    constexpr static GLfloat DEFAULT_NEAR = 0.1f;
    constexpr static GLfloat DEFAULT_FAR = 100.0f;

    constexpr static glm::vec3 DEFAULT_UP = glm::vec3(0.0f, 1.0f, 0.0f);
    constexpr static glm::vec3 DEFAULT_FRONT = glm::vec3(0.0f, 0.0f, -1.0f);
    constexpr static glm::vec3 DEFAULT_RIGHT = glm::vec3(1.0f, 0.0f, 0.0f);
    constexpr static glm::vec3 DEFAULT_POS = glm::vec3(0.0f, 0.0f, 0.0f);
    constexpr static glm::vec3 DEFAULT_TARGET = glm::vec3(0.0f, 0.0f, 0.0f);

    constexpr static GLfloat DEFAULT_YAW = -90.0f;
    constexpr static GLfloat DEFAULT_PITCH = 0.0f;
};

}
