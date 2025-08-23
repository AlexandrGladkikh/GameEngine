#pragma once

#include <memory>
#include <optional>

class QWidget;
class QTreeWidget;
class QTreeWidgetItem;

namespace engine {
class Component;
class TransformComponent;
class MaterialComponent;
class MeshComponent;
class CameraComponent;
class FlipbookAnimationComponent;
}

namespace editor {

class NodeWidget;
class ComponentWidget;
class SceneNodeTree;

class NodeTreeWidgetBuilder final {
public:
    explicit NodeTreeWidgetBuilder(SceneNodeTree* scene_node_tree);

    auto buildWidgetForNode(const std::string& node_name) -> std::optional<NodeWidget*>;
    auto buildWidgetForComponent(std::shared_ptr<engine::Component> component, QTreeWidgetItem* item) -> std::optional<ComponentWidget*>;

private:
    ComponentWidget* buildTransformWidget(const std::shared_ptr<engine::TransformComponent>& transform);
    ComponentWidget* buildMaterialWidget(const std::shared_ptr<engine::MaterialComponent>& material);
    ComponentWidget* buildMeshWidget(const std::shared_ptr<engine::MeshComponent>& mesh);
    ComponentWidget* buildCameraWidget(const std::shared_ptr<engine::CameraComponent>& camera);
    ComponentWidget* buildFlipbookAnimationWidget(const std::shared_ptr<engine::FlipbookAnimationComponent>& animation, QTreeWidgetItem* item);

    SceneNodeTree* m_scene_node_tree;
};

}
