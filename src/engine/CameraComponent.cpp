#include "CameraComponent.h"

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

namespace engine {

CameraComponent::CameraComponent(uint32_t id, const std::string& name, uint32_t owner_node, uint32_t owner_scene) :
    Component(id, name, owner_node, owner_scene),
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

void CameraComponent::init()
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

std::string CameraComponent::type() const
{
    return "camera";
}

void CameraComponent::setOrtho(GLfloat left, GLfloat right, GLfloat top, GLfloat bottom, GLfloat near, GLfloat far)
{
    m_ortho.left = left;
    m_ortho.right = right;
    m_ortho.top = top;
    m_ortho.bottom = bottom;
    m_ortho.near = near;
    m_ortho.far = far;
    m_projection = glm::ortho(left, right, bottom, top, near, far);
}

void CameraComponent::setOrtho(const Ortho &ortho)
{
    m_ortho = ortho;
    m_projection = glm::ortho(ortho.left, ortho.right, ortho.bottom, ortho.top, ortho.near, ortho.far);
}

auto CameraComponent::getOrtho() const -> Ortho
{
    return m_ortho;
}

auto CameraComponent::getView() const -> glm::mat4
{
    return m_view;
}

auto CameraComponent::getProjection() const -> glm::mat4
{
    return m_projection;
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
