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
class EngineObserver;

class UserTreeWidgetBuilder {
public:
    UserTreeWidgetBuilder() = default;
    virtual ~UserTreeWidgetBuilder() = default;

    [[nodiscard]]
    virtual std::optional<ComponentWidget*> buildWidgetForComponent(const std::shared_ptr<engine::Component>& component) const = 0;

    SceneNodeTree* sceneNodeTree() const;
    std::shared_ptr<EngineObserver> engineObserver() const;

private:
    void setSceneNodeTree(SceneNodeTree* scene_node_tree);
    void setEngineObserver(const std::shared_ptr<EngineObserver>& engine_observer);

    SceneNodeTree* m_scene_node_tree;

    std::shared_ptr<EngineObserver> m_engine_observer;

    friend class SceneNodeTree;
};

}
