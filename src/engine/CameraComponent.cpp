#include "CameraComponent.h"
#include "Utils.h"
#include "Logger.h"

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

namespace engine {

CameraComponent::CameraComponent(uint32_t id, const std::string& name, uint32_t owner_node, uint32_t owner_scene) :
    Component(id, name, owner_node, owner_scene),
    m_projection_type(ProjectionType::Orthographic),
    m_ortho(Ortho()),
    m_position(DEFAULT_POS),
    m_front(DEFAULT_FRONT),
    m_up(DEFAULT_UP),
    m_right(DEFAULT_RIGHT),
    m_world_up(DEFAULT_UP),
    m_yaw(DEFAULT_YAW),
    m_pitch(DEFAULT_PITCH),
    m_fov(DEFAULT_FOV),
    m_aspect(DEFAULT_ASPECT),
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

auto CameraComponent::type() const -> std::string
{
    return "camera";
}

auto CameraComponent::projectionType() const -> CameraComponent::ProjectionType
{
    return m_projection_type;
}

void CameraComponent::setProjectionType(CameraComponent::ProjectionType type)
{
    m_projection_type = type;
    if (type == ProjectionType::Orthographic) {
        m_projection = glm::ortho(m_ortho.left, m_ortho.right, m_ortho.bottom, m_ortho.top, m_near, m_far);
    } else {
        m_projection = glm::perspective(glm::radians(m_fov), m_aspect, m_near, m_far);
    }
    markDirty();
}

auto CameraComponent::clone(uint32_t owner_node_id) const -> std::unique_ptr<Component>
{
    auto clone_component = std::make_unique<CameraComponent>(generateUniqueId(), name(), owner_node_id, ownerScene());
    clone_component->setContext(context());
    clone_component->setValid(isValid());
    clone_component->setActive(isActive());
    clone_component->markDirty();

    clone_component->setPosition(m_position);
    clone_component->setYaw(m_yaw);
    clone_component->setPitch(m_pitch);
    clone_component->setOrtho(m_ortho);
    clone_component->setProjectionType(m_projection_type);

    return clone_component;
}

void CameraComponent::setOrtho(GLfloat left, GLfloat right, GLfloat top, GLfloat bottom, GLfloat near, GLfloat far)
{
    if (m_projection_type != ProjectionType::Orthographic) {
        Logger::error("CameraComponent::setOrtho: Projection type is not orthographic");
        return;
    }

    m_ortho.left = left;
    m_ortho.right = right;
    m_ortho.top = top;
    m_ortho.bottom = bottom;
    m_near = near;
    m_far = far;
    markDirty();
    m_projection = glm::ortho(left, right, bottom, top, m_near, m_far);
}

void CameraComponent::setOrtho(const Ortho &ortho)
{
    if (m_projection_type != ProjectionType::Orthographic) {
        Logger::error("CameraComponent::setOrtho: Projection type is not orthographic");
        return;
    }

    m_ortho = ortho;
    markDirty();
    m_projection = glm::ortho(ortho.left, ortho.right, ortho.bottom, ortho.top, m_near, m_far);
}

auto CameraComponent::getOrtho() const -> Ortho
{
    if (m_projection_type != ProjectionType::Orthographic) {
        Logger::error("CameraComponent::getOrtho: Projection type is not orthographic");
        return Ortho();
    }

    return m_ortho;
}

void CameraComponent::setFov(GLfloat fov)
{
    if (m_projection_type != ProjectionType::Perspective) {
        Logger::error("CameraComponent::setFov: Projection type is not perspective");
        return;
    }

    m_fov = fov;
    markDirty();
    m_projection = glm::perspective(glm::radians(m_fov), m_aspect, m_near, m_far);
}

auto CameraComponent::getFov() const -> GLfloat
{
    return m_fov;
}

void CameraComponent::setNear(GLfloat near)
{
    if (m_projection_type != ProjectionType::Perspective) {
        Logger::error("CameraComponent::setNear: Projection type is not perspective");
        return;
    }

    m_near = near;
    markDirty();
    if (m_projection_type == ProjectionType::Perspective) {
        m_projection = glm::perspective(glm::radians(m_fov), m_aspect, m_near, m_far);
    } else {
        m_projection = glm::ortho(m_ortho.left, m_ortho.right, m_ortho.bottom, m_ortho.top, m_near, m_far);
    }
}

auto CameraComponent::getNear() const -> GLfloat
{
    return m_near;
}

void CameraComponent::setFar(GLfloat far)
{
    if (m_projection_type != ProjectionType::Perspective) {
        Logger::error("CameraComponent::setFar: Projection type is not perspective");
        return;
    }

    m_far = far;
    markDirty();
    if (m_projection_type == ProjectionType::Perspective) {
        m_projection = glm::perspective(glm::radians(m_fov), m_aspect, m_near, m_far);
    } else {
        m_projection = glm::ortho(m_ortho.left, m_ortho.right, m_ortho.bottom, m_ortho.top, m_near, m_far);
    }
}

auto CameraComponent::getFar() const -> GLfloat
{
    return m_far;
}

void CameraComponent::setAspect(GLfloat aspect)
{
    m_aspect = aspect;
    markDirty();
    m_projection = glm::perspective(glm::radians(m_fov), m_aspect, m_near, m_far);
}

auto CameraComponent::getAspect() const -> GLfloat
{
    return m_aspect;
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
