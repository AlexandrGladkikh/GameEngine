#include "SceneRequester.h"

#include "Scene.h"
#include "SceneRequesterHelper.h"
#include "Context.h"
#include "TransformComponent.h"
#include "MeshComponent.h"
#include "MaterialComponent.h"
#include "CameraComponent.h"
#include "Node.h"

namespace engine {

SceneRequester::SceneSlice::SceneSlice(const std::shared_ptr<Scene>& scene, const std::shared_ptr<Context>& context, const std::unordered_map<uint32_t, std::shared_ptr<Node>>& nodes) :
    m_context(context), m_scene(scene)
{
    for (auto& [id, node] : nodes) {
        m_nodes.push_back(node);
    }
}

auto SceneRequester::SceneSlice::GetNodes(ComponentType type) const -> SceneSlice
{
    SceneSlice scene_slice(m_scene, m_context, {});

    switch (type) {
        case ComponentType::Transform:
            for (const auto& node : m_nodes) {
                if (SceneRequesterHelper::hasComponent<TransformComponent>(m_scene, node->components())) {
                    scene_slice.addNode(node);
                }
            }
            break;
        case ComponentType::Mesh:
            for (const auto& node : m_nodes) {
                if (SceneRequesterHelper::hasComponent<MeshComponent>(m_scene, node->components())) {
                    scene_slice.addNode(node);
                }
            }
            break;
        case ComponentType::Material:
            for (const auto& node : m_nodes) {
                if (SceneRequesterHelper::hasComponent<MaterialComponent>(m_scene, node->components())) {
                    scene_slice.addNode(node);
                }
            }
            break;
        case ComponentType::Camera:
            for (const auto& node : m_nodes) {
                if (SceneRequesterHelper::hasComponent<CameraComponent>(m_scene, node->components())) {
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
    return SceneSlice(scene, m_context, scene->getNodes()).GetNodes(type);
}

}
