#pragma once

#include <memory>
#include <optional>

namespace engine {
    class Scene;
}

class QWidget;
class QTreeWidget;

namespace editor {

class UserComponentBuilder;

class SceneNodeTree final {
public:
    explicit SceneNodeTree(QWidget* parent = nullptr);
    ~SceneNodeTree();

    void setUserComponentsBuilder(std::unique_ptr<UserComponentBuilder> builder);

    void build(std::optional<std::shared_ptr<engine::Scene>> scene);

    void show();

private:
    QTreeWidget* m_scene_tree;

    std::unique_ptr<UserComponentBuilder> m_user_components_builder;
};

}
