#include "BaseRenderPass.h"

#include "Logger.h"
#include "Context.h"
#include "Scene.h"
#include "Node.h"
#include "MeshComponent.h"
#include "MaterialComponent.h"
#include "TransformComponent.h"
#include "CameraComponent.h"
#include "RenderScopeComponent.h"
#include "ShaderStore.h"
#include "Shader.h"
#include "TextureStore.h"
#include "Texture.h"
#include "MeshStore.h"

#include <glm/ext/matrix_transform.hpp>

namespace engine {

glm::mat4 transformTune(const glm::mat4& transform, GLuint width, GLuint height)
{
    return glm::scale(transform, glm::vec3(width, height, 1.0f));
}

void BaseRenderPass::render(const std::shared_ptr<Context>& context,
                            const std::shared_ptr<Scene>& scene,
                            const std::shared_ptr<Node>& node,
                            const std::shared_ptr<CameraComponent>& camera) const
{
    Logger::info(__FUNCTION__);

    auto mesh = SceneRequesterHelper::getComponent<MeshComponent>(scene, node->components());
    auto material = SceneRequesterHelper::getComponent<MaterialComponent>(scene, node->components());
    auto transform = SceneRequesterHelper::getComponent<TransformComponent>(scene, node->components());

    if (!mesh.has_value() || !mesh.value()->isActive() || !mesh.value()->isValid() ||
        !material.has_value() || !material.value()->isActive() || !material.value()->isValid() ||
        !transform.has_value() || !transform.value()->isActive() || !transform.value()->isValid()) {
        return;
    }

    auto shader_id = material.value()->shaderId();
    auto shader_program = context->shaderStore->get(shader_id);
    if (!shader_program.has_value()) {
        return;
    }

    auto shader_program_value = shader_program.value();

    auto texture_id = material.value()->textureId();
    auto texture = context->textureStore->get(texture_id);
    if (!texture.has_value()) {
        return;
    }

    auto model_mtx = transform.value()->getModel();

    glm::vec3 absolute_node_position = transform.value()->getPosition();

    auto parent = node->getParentNode();
    while (parent.has_value()) {
        if (!parent.value()->hasComponent<TransformComponent>()) {
            parent = parent.value()->getParentNode();
            continue;
        }

        auto transform_parent = parent.value()->getComponent<TransformComponent>();
        if (!transform_parent.has_value() || !transform_parent.value()->isActive()) {
            break;
        }

        model_mtx = transform_parent.value()->getModel() * model_mtx;
        absolute_node_position += transform_parent.value()->getPosition();
        parent = parent.value()->getParentNode();
    }

    auto node_scale = transform.value()->getScale();
    auto texture_size = material.value()->textureSize();
    texture_size.first *= std::fabs(node_scale.x) / 2.0f;
    texture_size.second *= std::fabs(node_scale.y) / 2.0f;

    float absoluteNodePositionX = absolute_node_position.x - camera->getPosition().x;
    float absoluteNodePositionY = absolute_node_position.y - camera->getPosition().y;

    if (camera->projectionType() == CameraComponent::ProjectionType::Orthographic) {
        auto ortho = camera->getOrtho();

        if ((absoluteNodePositionX + texture_size.first < 0 || absoluteNodePositionX - texture_size.first > (ortho.right)) ||
            (absoluteNodePositionY + texture_size.second < 0 || absoluteNodePositionY - texture_size.second > (ortho.top))) {
            Logger::info("skip render node: {}", node->name());
            return;
        }
    }

    auto render_scope_component = SceneRequesterHelper::getComponent<RenderScopeComponent>(scene, node->components());
    if (!render_scope_component.has_value()) {
        return;
    }

    auto transform_mtx = model_mtx;
    if (render_scope_component.value()->isSprite()) {
        transform_mtx = transformTune(model_mtx, texture.value()->width(), texture.value()->height());
    }

    shader_program_value->use();
    shader_program_value->setUniform4mat("model", transform_mtx);
    shader_program_value->setUniform4mat("view", camera->getView());
    shader_program_value->setUniform4mat("projection", camera->getProjection());

    for (const auto& uniform : shader_program_value->uniforms()) {
        const auto uniform_value_opt = render_scope_component.value()->renderData(uniform.name);
        if (!uniform_value_opt.has_value()) {
            continue;
        }
        const auto& uniform_value = uniform_value_opt.value();
        if (uniform.type == Uniform::Type::Float && uniform_value.type() == typeid(float)) {
            shader_program_value->setUniform1f(uniform.name, std::any_cast<float>(uniform_value));
        } else if (uniform.type == Uniform::Type::Double && uniform_value.type() == typeid(double)) {
            shader_program_value->setUniform1d(uniform.name, std::any_cast<double>(uniform_value));
        } else if (uniform.type == Uniform::Type::Int && uniform_value.type() == typeid(int)) {
            shader_program_value->setUniform1i(uniform.name, std::any_cast<int>(uniform_value));
        } else if (uniform.type == Uniform::Type::UInt && uniform_value.type() == typeid(uint32_t)) {
            shader_program_value->setUniform1ui(uniform.name, std::any_cast<uint32_t>(uniform_value));
        } else if (uniform.type == Uniform::Type::Bool && uniform_value.type() == typeid(bool)) {
            shader_program_value->setUniform1b(uniform.name, std::any_cast<bool>(uniform_value));
        } else if (uniform.type == Uniform::Type::Vec2 && uniform_value.type() == typeid(glm::vec2)) {
            shader_program_value->setUniform2vec(uniform.name, std::any_cast<glm::vec2>(uniform_value));
        } else if (uniform.type == Uniform::Type::Vec3 && uniform_value.type() == typeid(glm::vec3)) {
            shader_program_value->setUniform3vec(uniform.name, std::any_cast<glm::vec3>(uniform_value));
        } else if (uniform.type == Uniform::Type::Vec4 && uniform_value.type() == typeid(glm::vec4)) {
            shader_program_value->setUniform4vec(uniform.name, std::any_cast<glm::vec4>(uniform_value));
        } else if (uniform.type == Uniform::Type::Mat2 && uniform_value.type() == typeid(glm::mat2)) {
            shader_program_value->setUniform2mat(uniform.name, std::any_cast<glm::mat2>(uniform_value));
        } else if (uniform.type == Uniform::Type::Mat3 && uniform_value.type() == typeid(glm::mat3)) {
                shader_program_value->setUniform3mat(uniform.name, std::any_cast<glm::mat3>(uniform_value));
        } else if (uniform.type == Uniform::Type::Mat4 && uniform_value.type() == typeid(glm::mat4)) {
            shader_program_value->setUniform4mat(uniform.name, std::any_cast<glm::mat4>(uniform_value));
        }
    }

    glActiveTexture(GL_TEXTURE0);
    texture.value()->bind();
    shader_program_value->setUniform1i("texture1", 0);

    mesh.value()->bind();

    auto mesh_data = context->meshStore->get(mesh.value()->meshId());
    if (!mesh_data.has_value()) {
        return;
    }

    glDrawElements(GL_TRIANGLES, mesh_data.value()->indices.size(),  GL_UNSIGNED_INT, nullptr);
}

}
