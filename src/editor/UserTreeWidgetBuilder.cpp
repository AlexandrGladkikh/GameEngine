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

void UserTreeWidgetBuilder::setSceneNodeTree(SceneNodeTree* scene_node_tree) {
    m_scene_node_tree = scene_node_tree;
}

void UserTreeWidgetBuilder::setEngineObserver(const std::shared_ptr<EngineObserver>& engine_observer) {
    m_engine_observer = engine_observer;
}

}
