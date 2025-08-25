#include "UserTreeWidgetBuilder.h"

namespace editor {

SceneNodeTree* UserTreeWidgetBuilder::sceneNodeTree() const
{
    return m_scene_node_tree;
}

std::shared_ptr<EngineObserver> UserTreeWidgetBuilder::engineObserver() const
{
    return m_engine_observer;
}

std::shared_ptr<TreeWidgetBuilderHelper> UserTreeWidgetBuilder::treeWidgetBuilderHelper() const
{
    return m_tree_widget_builder_helper;
}

void UserTreeWidgetBuilder::setSceneNodeTree(SceneNodeTree* scene_node_tree) {
    m_scene_node_tree = scene_node_tree;
}

void UserTreeWidgetBuilder::setEngineObserver(const std::shared_ptr<EngineObserver>& engine_observer) {
    m_engine_observer = engine_observer;
}

void UserTreeWidgetBuilder::setTreeWidgetBuilderHelper(const std::shared_ptr<TreeWidgetBuilderHelper>& tree_widget_builder_helper)
{
    m_tree_widget_builder_helper = tree_widget_builder_helper;
}

}
