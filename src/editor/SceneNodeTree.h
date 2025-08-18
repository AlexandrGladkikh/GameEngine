#pragma once

#include <QMainWindow>

#include <memory>
#include <optional>
#include <set>

namespace engine {
class Scene;
class Engine;
}

class QWidget;
class QTreeWidget;
class QMenu;
class QTreeWidgetItem;
class QAction;

namespace editor {

class UserNodeTreeBuilder;

class SceneNodeTree final : public QMainWindow {
public:
    explicit SceneNodeTree(engine::Engine* engine, QWidget* parent = nullptr);
    ~SceneNodeTree() override;

    void setUserComponentsBuilder(std::unique_ptr<UserNodeTreeBuilder> builder);

    void build(std::optional<std::shared_ptr<engine::Scene>> scene);

private slots:
    void onHeaderContextMenu(const QPoint& pos);
    void onAddRootNode();

    void onCustomContextMenu(const QPoint& pos);
    void onAddChildNode();
    void onRemoveNode();
    void onAddComponent();
    void onRemoveComponent();

    void onSaveScene();

private:
    void initMenuBar();

    void initHeaderContextMenu();
    void initContextMenu();

    QTreeWidget* m_scene_tree;

    QTreeWidgetItem* m_selected_item;

    QMenu* m_header_context_menu;

    QAction* m_add_root_node_action;

    QMenu* m_context_menu;

    QAction* m_add_child_node_action;
    QAction* m_remove_node_action;
    QMenu* m_add_component_menu;
    std::map<std::string, QAction*> m_add_component_actions;
    QAction* m_remove_component_action;

    std::unique_ptr<UserNodeTreeBuilder> m_user_components_builder;

    engine::Engine* m_engine;
};

}
