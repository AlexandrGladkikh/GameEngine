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

namespace engine {

void Renderer::render(const std::shared_ptr<Context>& context, const std::shared_ptr<Scene>& scene)
{
    static auto requester = SceneRequester(context);
    auto nodes = requester.GetNodes(scene, ComponentType::Mesh).
                                                 GetNodes(ComponentType::Material).
                                                 GetNodes(ComponentType::Transform).Unwrap();

    for (const auto& node : nodes) {
        auto mesh = getComponent<MeshComponent>(scene, node);
        auto material = getComponent<MaterialComponent>(scene, node);
        auto transform = getComponent<TransformComponent>(scene, node);
        auto camera = getComponent<CameraComponent>(scene, node);

        if (!mesh.has_value() || !material.has_value() || !transform.has_value() || !camera.has_value()) {
            continue;
        }

        auto shader_id = material.value()->shader();
        auto shader_program = context->shaderStore->get(shader_id);
        if (!shader_program.has_value()) {
            continue;
        }

        shader_program.value()->use();
        shader_program.value()->setUniform4mat("model", transform.value()->getModel());
        shader_program.value()->setUniform4mat("view", camera.value()->getView());
        shader_program.value()->setUniform4mat("projection", camera.value()->getProjection());

        auto texture_id = material.value()->texture();
        auto texture = context->textureStore->get(texture_id);
        if (!texture.has_value()) {
            continue;
        }

        texture.value()->bind();

        shader_program.value()->setUniform1i("texture", 0);

        mesh.value()->bind();

        auto mesh_data = context->meshStore->get(mesh.value()->meshId());
        if (!mesh_data.has_value()) {
            continue;
        }

        glDrawArrays(GL_TRIANGLES, 0, mesh_data.value()->vertices.size());
    }
}

}