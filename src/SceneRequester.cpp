#include "SceneRequester.h"

#include "Scene.h"
#include "SceneRequesterHelper.h"
#include "Context.h"
#include "TransformComponent.h"
#include "MeshComponent.h"
#include "MaterialComponent.h"
#include "CameraComponent.h"

namespace engine {

SceneRequester::SceneSlice::SceneSlice(const std::shared_ptr<Context>& context, const std::shared_ptr<Scene>& scene) :
        m_context(context), m_scene(scene)
{

}

auto SceneRequester::SceneSlice::GetNodes(ComponentType type) const -> SceneSlice
{
    SceneSlice scene_slice(m_context, m_scene);

    switch (type) {
        case ComponentType::Transform:
            for (const auto& [id, node] : m_scene->getNodes()) {
                if (hasComponent<TransformComponent>(m_scene, node)) {
                    scene_slice.addNode(node);
                }
            }
            break;
        case ComponentType::Mesh:
            for (const auto& [id, node] : m_scene->getNodes()) {
                if (hasComponent<MeshComponent>(m_scene, node)) {
                    scene_slice.addNode(node);
                }
            }
            break;
        case ComponentType::Material:
            for (const auto& [id, node] : m_scene->getNodes()) {
                if (hasComponent<MaterialComponent>(m_scene, node)) {
                    scene_slice.addNode(node);
                }
            }
            break;
        case ComponentType::Camera:
            for (const auto& [id, node] : m_scene->getNodes()) {
                if (hasComponent<CameraComponent>(m_scene, node)) {
                    scene_slice.addNode(node);
                }
            }
    }

    return scene_slice;
}

auto SceneRequester::SceneSlice::Unwrap() const -> std::vector<std::shared_ptr<Node>>
{
    return m_nodes;
}

void SceneRequester::SceneSlice::addNode(const std::shared_ptr<Node>& node)
{
    m_nodes.push_back(node);
}

SceneRequester::SceneRequester(const std::shared_ptr<Context>& context) :
    m_context(context)
{

}

auto SceneRequester::GetNodes(const std::shared_ptr<Scene>& scene, ComponentType type) -> SceneSlice
{
    return SceneSlice(m_context, scene).GetNodes(type);
}

}
