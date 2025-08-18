#include "SceneNodeTree.h"
#include "NodeTreeBuilder.h"
#include "UserNodeTreeBuilder.h"
#include "ComponentWidget.h"
#include "NodeWidget.h"

#include "engine/Engine.h"
#include "engine/Scene.h"
#include "engine/Node.h"
#include "engine/Component.h"
#include "engine/Context.h"
#include "engine/ComponentBuilder.h"
#include "engine/UserComponentsBuilder.h"

#include <QTreeWidget>
#include <QMenu>
#include <QAction>
#include <QInputDialog>
#include <QHeaderView>
#include <QMenuBar>

#include <functional>

namespace editor {

SceneNodeTree::SceneNodeTree(engine::Engine* engine, QWidget* parent) :
    QMainWindow(parent),
    m_scene_tree(new QTreeWidget(parent)),
    m_engine(engine)
{
    setWindowTitle("Scene node tree");
    resize(500, 800);

    initMenuBar();

    setCentralWidget(m_scene_tree);

    m_scene_tree->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_scene_tree, &QTreeWidget::customContextMenuRequested, this, &SceneNodeTree::onCustomContextMenu);

    auto header = m_scene_tree->header();
    header->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(header, &QHeaderView::customContextMenuRequested, this, &SceneNodeTree::onHeaderContextMenu);

    initContextMenu();
    initHeaderContextMenu();
}

SceneNodeTree::~SceneNodeTree() = default;

void SceneNodeTree::setUserComponentsBuilder(std::unique_ptr<UserNodeTreeBuilder> builder)
{
    m_user_components_builder = std::move(builder);
}


void SceneNodeTree::onHeaderContextMenu(const QPoint& pos)
{
    int logical_index = m_scene_tree->header()->logicalIndexAt(pos);
    if (logical_index < 0) {
        return;
    }

    m_header_context_menu->popup(m_scene_tree->viewport()->mapToGlobal(pos));
}

void SceneNodeTree::onAddRootNode()
{
    bool ok = false;
    QString root_node_name = QInputDialog::getText(this, "Add root node",
        "Node name", QLineEdit::Normal, "New root node", &ok);

    if (!ok || root_node_name.isEmpty()) {
        return;
    }

    auto ctx = m_engine->context();
    auto active_scene_id = m_engine->getActiveSceneId();

    auto active_scene = ctx->sceneStore->get(active_scene_id);
    if (!active_scene.has_value()) {
        return;
    }

    auto new_root_node = active_scene.value()->createRootNode(root_node_name.toStdString());

    if (!new_root_node.has_value()) {
        return;
    }

    auto new_root_node_value = new_root_node.value();

    QTreeWidgetItem* nodeItem = new QTreeWidgetItem(m_scene_tree);

    auto new_node_widget = NodeTreeBuilder::buildWidgetForNode(new_root_node_value->name());
    if (!new_node_widget.has_value()) {
        return;
    }

    new_node_widget.value()->setNode(new_root_node_value);
    m_scene_tree->setItemWidget(nodeItem, 0, new_node_widget.value());
}

void SceneNodeTree::onCustomContextMenu(const QPoint& pos)
{
    QTreeWidgetItem* item = m_scene_tree->itemAt(pos);
    if (item == nullptr) {
        return;
    }

    m_selected_item = item;

    auto widget = m_scene_tree->itemWidget(item, 0);

    bool is_node_widget = dynamic_cast<NodeWidget*>(widget) != nullptr;
    bool is_component_widget = dynamic_cast<ComponentWidget*>(widget) != nullptr;

    if (is_node_widget) {
        m_add_child_node_action->setEnabled(true);
        m_remove_node_action->setEnabled(true);
        m_add_component_menu->setEnabled(true);
        m_remove_component_action->setEnabled(false);
    }

    if (is_component_widget) {
        m_add_child_node_action->setEnabled(false);
        m_remove_node_action->setEnabled(false);
        m_add_component_menu->setEnabled(false);
        m_remove_component_action->setEnabled(true);
    }

    m_context_menu->popup(m_scene_tree->viewport()->mapToGlobal(pos));
}

void SceneNodeTree::onAddChildNode()
{
    bool ok = false;
    QString new_node_name = QInputDialog::getText(this, "Add child node",
        "Node name", QLineEdit::Normal, "New node", &ok);

    if (!ok || new_node_name.isEmpty()) {
        return;
    }

    auto widget = m_scene_tree->itemWidget(m_selected_item, 0);
    NodeWidget* node_widget = dynamic_cast<NodeWidget*>(widget);
    auto new_node = node_widget->node()->addChild(new_node_name.toStdString());

    QTreeWidgetItem* nodeItem = new QTreeWidgetItem(m_selected_item);

    auto new_node_widget = NodeTreeBuilder::buildWidgetForNode(new_node->name());
    if (!new_node_widget.has_value()) {
        return;
    }

    new_node_widget.value()->setNode(new_node);
    m_scene_tree->setItemWidget(nodeItem, 0, new_node_widget.value());

    m_selected_item = nullptr;
}

void SceneNodeTree::onRemoveNode()
{
    auto widget = m_scene_tree->itemWidget(m_selected_item, 0);
    NodeWidget* node_widget = dynamic_cast<NodeWidget*>(widget);

    node_widget->node()->getScene().value()->removeNode(node_widget->node()->id());
    delete m_selected_item;

    m_selected_item = nullptr;
}

void SceneNodeTree::onAddComponent()
{
    auto action = qobject_cast<QAction*>(sender());
    if (action == nullptr) {
        return;
    }

    bool ok = false;
    QString new_component_name = QInputDialog::getText(this, "Add component",
        "Component name", QLineEdit::Normal, action->text(), &ok);

    if (!ok || new_component_name.isEmpty()) {
        return;
    }

    auto widget = m_scene_tree->itemWidget(m_selected_item, 0);
    auto node = dynamic_cast<NodeWidget*>(widget)->node();
    auto component_type = action->text().toStdString();

    auto component = node->addComponent(component_type, new_component_name.toStdString());

    if (!component.has_value()) {
        return;
    }

    auto component_value = component.value();

    auto component_widget = NodeTreeBuilder::buildWidgetForComponent(component_value);
    if (!component_widget.has_value()) {
        return;
    }

    auto* componentItem = new QTreeWidgetItem(m_selected_item);

    m_scene_tree->setItemWidget(componentItem, 0, component_widget.value());

    m_selected_item = nullptr;
}

void SceneNodeTree::onRemoveComponent()
{
    auto widget = m_scene_tree->itemWidget(m_selected_item, 0);
    auto* component_widget = dynamic_cast<ComponentWidget*>(widget);
    auto node = component_widget->component()->getNode();
    if (!node.has_value()) {
        return;
    }
    node.value()->removeComponent(component_widget->component()->id());
    delete m_selected_item;

    m_selected_item = nullptr;
}

void SceneNodeTree::onSaveScene()
{
    m_engine->saveScene(m_engine->getActiveSceneId());
}

void SceneNodeTree::build(std::optional<std::shared_ptr<engine::Scene>> scene)
{
    if (!scene.has_value()) {
        return;
    }

    auto& scene_value = scene.value();

    m_scene_tree->clear();

    m_scene_tree->setHeaderLabel(scene_value->name().c_str());

    auto root_node = scene_value->getRoot();

    if (!root_node.has_value()) {
        return;
    }

    std::function<void(std::optional<std::shared_ptr<engine::Node>>, QTreeWidgetItem*)> handle_node = [&](std::optional<std::shared_ptr<engine::Node>> node, QTreeWidgetItem* parent) {
        if (!node.has_value()) {
            return;
        }

        auto& node_value = node.value();

        QTreeWidgetItem* nodeItem = nullptr;

        if (parent == nullptr) {
            nodeItem = new QTreeWidgetItem(m_scene_tree);
        } else {
            nodeItem = new QTreeWidgetItem(parent);
        }

        auto node_widget = NodeTreeBuilder::buildWidgetForNode(node_value->name());
        if (!node_widget.has_value()) {
            return;
        }

        node_widget.value()->setNode(node_value);
        m_scene_tree->setItemWidget(nodeItem, 0, node_widget.value());

        auto components = node_value->components();

        for (const auto component_id : components) {
            auto component = node_value->getComponent(component_id);

            if (!component.has_value()) {
                return;
            }

            auto& component_value = component.value();

            auto* componentItem = new QTreeWidgetItem(nodeItem);
            auto component_widget = NodeTreeBuilder::buildWidgetForComponent(component_value);
            if (!component_widget.has_value()) {
                component_widget = m_user_components_builder->buildWidgetForComponent(component_value);
            }
            if (component_widget.has_value()) {
                component_widget.value()->setComponent(component_value);
                m_scene_tree->setItemWidget(componentItem, 0, component_widget.value());
            }
        }

        auto node_children = node_value->children();
        for (const auto child_id : node_children) {
            handle_node(scene_value->getNode(child_id), nodeItem);
        }
    };

    handle_node(root_node, nullptr);

    m_scene_tree->expandAll();
}

void SceneNodeTree::initMenuBar()
{
    QMenu* fileMenu = menuBar()->addMenu("&File");
    fileMenu->addAction("&Save Scane", this, &SceneNodeTree::onSaveScene);
}

void SceneNodeTree::initHeaderContextMenu()
{
    m_header_context_menu = new QMenu(m_scene_tree);

    m_add_root_node_action = m_header_context_menu->addAction("Add root node");
    m_add_root_node_action->setIcon(QIcon::fromTheme("list-add"));
    connect(m_add_root_node_action, &QAction::triggered, this, &SceneNodeTree::onAddRootNode);
}

void SceneNodeTree::initContextMenu()
{
    m_context_menu = new QMenu(m_scene_tree);

    m_add_child_node_action = m_context_menu->addAction("Add child node");
    m_add_child_node_action->setIcon(QIcon::fromTheme("list-add"));
    connect(m_add_child_node_action, &QAction::triggered, this, &SceneNodeTree::onAddChildNode);

    m_remove_node_action = m_context_menu->addAction("Remove node");
    m_remove_node_action->setIcon(QIcon::fromTheme("list-remove"));
    connect(m_remove_node_action, &QAction::triggered, this, &SceneNodeTree::onRemoveNode);

    auto ctx = m_engine->context();

    std::vector<std::string> component_types = ctx->userComponentsBuilder->componentTypes();

    component_types.insert(component_types.end(), engine::ComponentBuilder::componentTypes().begin(), engine::ComponentBuilder::componentTypes().end());

    m_add_component_menu = new QMenu("Add components", m_context_menu);

    for (const auto& component_type : component_types) {
        m_add_component_actions[component_type] = m_add_component_menu->addAction(QString::fromStdString(component_type));
        m_add_component_actions[component_type]->setIcon(QIcon::fromTheme("list-add"));
        connect(m_add_component_actions[component_type], &QAction::triggered, this, &SceneNodeTree::onAddComponent);
    }

    m_context_menu->addMenu(m_add_component_menu);

    m_remove_component_action = m_context_menu->addAction("Remove component");
    m_remove_component_action->setIcon(QIcon::fromTheme("list-remove"));
    connect(m_remove_component_action, &QAction::triggered, this, &SceneNodeTree::onRemoveComponent);
}


}