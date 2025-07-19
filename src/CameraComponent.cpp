#include "CameraComponent.h"

#include <glm/ext/matrix_transform.hpp>

namespace engine {

CameraComponent::CameraComponent(uint32_t id, const std::string& name, uint32_t owner) :
    Component(id, name, owner),
    m_position(DEFAULT_POS),
    m_front(DEFAULT_FRONT),
    m_up(DEFAULT_UP),
    m_right(DEFAULT_RIGHT),
    m_world_up(DEFAULT_UP),
    m_yaw(DEFAULT_YAW),
    m_pitch(DEFAULT_PITCH),
    m_fov(DEFAULT_FOV),
    m_near(DEFAULT_NEAR),
    m_far(DEFAULT_FAR),
    m_view(glm::mat4(1.0f)),
    m_dirty(true)
{

}

void CameraComponent::update(uint64_t dt)
{

}

bool CameraComponent::isDirty() const
{
    return m_dirty;
}

void CameraComponent::markDirty()
{
    m_dirty = true;
}

void CameraComponent::clearDirty()
{
    m_dirty = false;
}

auto CameraComponent::getView() const -> glm::mat4
{
    return m_view;
}

void CameraComponent::setPosition(const glm::vec3& position)
{
    m_position = position;
    markDirty();
    updateView();
}

auto CameraComponent::getPosition() const -> glm::vec3
{
    return m_position;
}

void CameraComponent::setYaw(GLfloat yaw)
{
    m_yaw = yaw;
    markDirty();
    updateView();
}

void CameraComponent::setPitch(GLfloat pitch)
{
    m_pitch = pitch;
    markDirty();
    updateView();
}

auto CameraComponent::getYaw() const -> GLfloat
{
    return m_yaw;
}

auto CameraComponent::getPitch() const -> GLfloat
{
    return m_pitch;
}

void CameraComponent::updateView()
{
    glm::vec3 front;
    front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    front.y = sin(glm::radians(m_pitch));
    front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    m_front = glm::normalize(front);
    m_right = glm::normalize(glm::cross(m_front, m_world_up));
    m_up = glm::normalize(glm::cross(m_right, m_front));

    m_view = glm::lookAt(m_position, m_position + m_front, m_up);
}

}
