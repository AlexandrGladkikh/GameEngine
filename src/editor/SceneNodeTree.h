#pragma once

#include <QMainWindow>

#include <memory>
#include <optional>

namespace engine {
class Scene;
class Engine;
class Component;
class Node;
}

class QWidget;
class QTreeWidget;
class QMenu;
class QTreeWidgetItem;
class QAction;
class QTimer;

namespace editor {

class TreeWidgetBuilder;
class UserTreeWidgetBuilder;
class ResourcePackagesEditor;

class EngineObserver;

class SceneNodeTree final : public QMainWindow {
public:
    explicit SceneNodeTree(engine::Engine* engine, QWidget* parent = nullptr);
    ~SceneNodeTree() override;

    void setUserTreeWidgetBuilder(std::unique_ptr<UserTreeWidgetBuilder> builder);

    void build(std::optional<std::shared_ptr<engine::Scene>> scene);

    void rebuildComponentWidget(QTreeWidgetItem* item);

private slots:
    void onHeaderContextMenu(const QPoint& pos);
    void onAddRootNode();

    void onCustomContextMenu(const QPoint& pos);
    void onAddChildNode();
    void onRemoveNode();
    void onAddComponent();
    void onRemoveComponent();
    void onCopyId();
    void onCopyName();
    void onCopy();
    void onPaste();
    void onRename();
    void onDuplicate();
    void onEnable();
    void onDisable();

    void onSaveScene();

protected:
    void moveEvent(QMoveEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private:
    void initMenuBar();

    void initHeaderContextMenu();
    void initContextMenu();

    void createComponentWidget(QTreeWidgetItem* item, const std::shared_ptr<engine::Component>& component);
    void createNodeWidget(std::optional<std::shared_ptr<engine::Node>> node, QTreeWidgetItem* parent);

    void changeActiveWidget(bool active);

    void positioningEngineWindow();

    engine::Engine* m_engine;

    std::shared_ptr<EngineObserver> m_engine_observer;

    std::shared_ptr<TreeWidgetBuilder> m_scene_tree_widget_builder;
    std::unique_ptr<UserTreeWidgetBuilder> m_user_tree_widget_builder;

    ResourcePackagesEditor* m_resource_package_editor;

    QTreeWidget* m_scene_tree;

    QTreeWidgetItem* m_selected_item;
    QTreeWidgetItem* m_to_copy_item;

    QMenu* m_header_context_menu;

    QAction* m_add_root_node_action;

    QMenu* m_context_menu;

    QAction* m_add_child_node_action;
    QAction* m_remove_node_action;
    QMenu* m_add_component_menu;
    std::map<std::string, QAction*> m_add_component_actions;
    QAction* m_remove_component_action;
    QAction* m_copy_id;
    QAction* m_copy_name;
    QAction* m_copy;
    QAction* m_paste;
    QAction* m_rename;
    QAction* m_duplicate;
    QAction* m_enable;
    QAction* m_disable;
};

}
