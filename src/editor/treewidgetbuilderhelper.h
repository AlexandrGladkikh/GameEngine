#pragma once

#include <memory>
#include <functional>

namespace engine {
class Node;
class Component;
}

class QWidget;

namespace editor {

class EngineObserver;

class TreeWidgetBuilderHelper {
public:
    explicit TreeWidgetBuilderHelper(const std::shared_ptr<EngineObserver>& engine_observer);

    void subscribeOnActiveNode(QWidget* widget, const std::shared_ptr<engine::Node>& node);
    void subscribeOnActiveComponent(QWidget* widget, const std::shared_ptr<engine::Component>& component);

private:
    void subscribeOnActiveSceneItem(QWidget* widget, const std::function<bool()> updater);

    std::shared_ptr<EngineObserver> m_engine_observer;
};

}
