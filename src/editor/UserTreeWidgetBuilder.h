#pragma once

#include <memory>
#include <optional>

namespace engine {
class Component;
}

class QWidget;

namespace editor {

class ComponentWidget;
class SceneNodeTree;
class EngineController;
class TreeWidgetBuilderHelper;

class UserTreeWidgetBuilder {
public:
    UserTreeWidgetBuilder() = default;
    virtual ~UserTreeWidgetBuilder() = default;

    [[nodiscard]]
    virtual std::optional<ComponentWidget*> buildWidgetForComponent(const std::shared_ptr<engine::Component>& component) const = 0;

    SceneNodeTree* sceneNodeTree() const;
    std::shared_ptr<EngineController> engineObserver() const;
    std::shared_ptr<TreeWidgetBuilderHelper> treeWidgetBuilderHelper() const;

private:
    void setSceneNodeTree(SceneNodeTree* scene_node_tree);
    void setEngineObserver(const std::shared_ptr<EngineController>& engine_observer);
    void setTreeWidgetBuilderHelper(const std::shared_ptr<TreeWidgetBuilderHelper>& tree_widget_builder_helper);

    SceneNodeTree* m_scene_node_tree;

    std::shared_ptr<EngineController> m_engine_observer;

    std::shared_ptr<TreeWidgetBuilderHelper> m_tree_widget_builder_helper;

    friend class SceneNodeTree;
};

}
