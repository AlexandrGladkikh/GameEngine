#pragma once

#include <memory>
#include <optional>

class QWidget;
class QTreeWidget;
class QTreeWidgetItem;
class QLabel;

namespace engine {
class Component;
class TransformComponent;
class MaterialComponent;
class MeshComponent;
class CameraComponent;
class FlipbookAnimationComponent;
class MouseEventFilterComponent;
class RenderComponent;
class Node;
}

namespace editor {

class NodeWidget;
class ComponentWidget;
class SceneNodeTree;

class EngineController;
class TreeWidgetBuilderHelper;

class TreeWidgetBuilder final {
public:
    explicit TreeWidgetBuilder(SceneNodeTree* scene_node_tree, const std::shared_ptr<EngineController>& engine_controller);

    auto buildWidgetForNode(const std::shared_ptr<engine::Node>& node) -> std::optional<NodeWidget*>;
    auto buildWidgetForComponent(std::shared_ptr<engine::Component> component, QTreeWidgetItem* item) -> std::optional<ComponentWidget*>;

private:
    ComponentWidget* buildTransformWidget(const std::shared_ptr<engine::TransformComponent>& transform);
    ComponentWidget* buildMaterialWidget(const std::shared_ptr<engine::MaterialComponent>& material);
    ComponentWidget* buildMeshWidget(const std::shared_ptr<engine::MeshComponent>& mesh);
    ComponentWidget* buildCameraWidget(const std::shared_ptr<engine::CameraComponent>& camera, QTreeWidgetItem* item);
    ComponentWidget* buildFlipbookAnimationWidget(const std::shared_ptr<engine::FlipbookAnimationComponent>& animation, QTreeWidgetItem* item);
    ComponentWidget* buildMouseEventFilterWidget(const std::shared_ptr<engine::MouseEventFilterComponent>& mouse_event_filter);
    ComponentWidget* buildRenderWidget(const std::shared_ptr<engine::RenderComponent>& render);

    void decorateLabel(QLabel* label);

    SceneNodeTree* m_scene_node_tree;

    std::shared_ptr<EngineController> m_engine_controller;

    std::unique_ptr<TreeWidgetBuilderHelper> m_tree_widget_builder_helper;
};

}
