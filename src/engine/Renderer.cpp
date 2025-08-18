#include "Renderer.h"
#include "Context.h"
#include "Scene.h"
#include "SceneRequester.h"
#include "SceneRequesterHelper.h"
#include "MeshComponent.h"
#include "MaterialComponent.h"
#include "TransformComponent.h"
#include "CameraComponent.h"
#include "ShaderStore.h"
#include "Shader.h"
#include "TextureStore.h"
#include "Texture.h"
#include "MeshStore.h"
#include "Logger.h"
#include "Node.h"

#include <glm/ext/matrix_transform.hpp>

namespace engine {

glm::mat4 transformTune(const glm::mat4& transform, GLuint width, GLuint height)
{
    return glm::scale(transform, glm::vec3(width, height, 1.0f));
}

void Renderer::render(const std::shared_ptr<Context>& context, const std::shared_ptr<Scene>& scene)
{
    Logger::info(__FUNCTION__);

     glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    static auto requester = SceneRequester(context);
    auto nodes = requester.GetNodes(scene, ComponentType::Mesh).
                                                 GetNodes(ComponentType::Material).
                                                 GetNodes(ComponentType::Transform).Unwrap();

    auto camera_nodes = requester.GetNodes(scene, ComponentType::Camera).GetNodes(ComponentType::Transform).Unwrap();
    if (camera_nodes.empty()) {
        return;
    }

    auto camera_node = camera_nodes.front();

    if (!camera_node) {
        return;
    }

    auto camera_component = SceneRequesterHelper::getComponent<CameraComponent>(scene, camera_node->components());
    if (!camera_component.has_value()) {
        return;
    }

    auto camera_node_transform = SceneRequesterHelper::getComponent<TransformComponent>(scene, camera_node->components());
    if (!camera_node_transform.has_value()) {
        return;
    }

    auto camera = camera_component.value();
    camera->setPosition(camera_node_transform.value()->getPosition());

    for (const auto& node : nodes) {
        auto mesh = SceneRequesterHelper::getComponent<MeshComponent>(scene, node->components());
        auto material = SceneRequesterHelper::getComponent<MaterialComponent>(scene, node->components());
        auto transform = SceneRequesterHelper::getComponent<TransformComponent>(scene, node->components());

        if (!mesh.has_value() || !mesh.value()->isActive() || !mesh.value()->isValid() ||
            !material.has_value() || !material.value()->isActive() || !material.value()->isValid() ||
            !transform.has_value() || !transform.value()->isActive() || !transform.value()->isValid()) {
            continue;
        }

        auto shader_id = material.value()->shaderId();
        auto shader_program = context->shaderStore->get(shader_id);
        if (!shader_program.has_value()) {
            continue;
        }

        auto texture_id = material.value()->textureId();
        auto texture = context->textureStore->get(texture_id);
        if (!texture.has_value()) {
            continue;
        }

        auto model_mtx = transform.value()->getModel();

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
            parent = parent.value()->getParentNode();
        }

        auto transform_mtx = transformTune(model_mtx, texture.value()->width(), texture.value()->height());

        shader_program.value()->use();
        shader_program.value()->setUniform4mat("model", transform_mtx);
        shader_program.value()->setUniform4mat("view", camera->getView());
        shader_program.value()->setUniform4mat("projection", camera->getProjection());

        glActiveTexture(GL_TEXTURE0);
        texture.value()->bind();
        shader_program.value()->setUniform1i("texture1", 0);

        mesh.value()->bind();

        auto mesh_data = context->meshStore->get(mesh.value()->meshId());
        if (!mesh_data.has_value()) {
            continue;
        }

        glDrawElements(GL_TRIANGLES, mesh_data.value()->indices.size(),  GL_UNSIGNED_INT, nullptr);
    }
}

}