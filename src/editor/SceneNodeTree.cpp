#include "SceneNodeTree.h"
#include "NodeBuilder.h"
#include "UserComponentBuilder.h"

#include "engine/Scene.h"
#include "engine/Node.h"

#include <QTreeWidget>

#include <functional>

namespace editor {

SceneNodeTree::SceneNodeTree(QWidget* parent) :
    m_scene_tree(new QTreeWidget(parent))
{

}

SceneNodeTree::~SceneNodeTree() = default;

void SceneNodeTree::setUserComponentsBuilder(std::unique_ptr<UserComponentBuilder> builder)
{
    m_user_components_builder = std::move(builder);
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

    auto& root_node_value = root_node.value();

    // auto* nodeItem = new QTreeWidgetItem(m_scene_tree);
    // nodeItem->setText(0, root_node_value->name().c_str());

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

        nodeItem->setText(0, node_value->name().c_str());

        auto components = node_value->components();

        for (const auto component_id : components) {
            auto component = node_value->getComponent(component_id);

            if (!component.has_value()) {
                return;
            }

            auto& component_value = component.value();

            auto* componentItem = new QTreeWidgetItem(nodeItem);
            auto component_widget = NodeBuilder::buildComponent(component_value);
            if (component_widget == nullptr) {
                auto user_widget = m_user_components_builder->buildComponent(component_value);
                if (user_widget.has_value()) {
                    component_widget = user_widget.value();
                }
            }
            m_scene_tree->setItemWidget(componentItem, 0, component_widget);
        }

        auto node_children = node_value->children();
        for (const auto child_id : node_children) {
            handle_node(scene_value->getNode(child_id), nodeItem);
        }
    };

    handle_node(root_node, nullptr);
}

void SceneNodeTree::show()
{
    m_scene_tree->expandAll();
    m_scene_tree->show();
}

}