#pragma once

#include <memory>
#include <functional>

namespace engine {
class Node;
class Component;
}

class QWidget;

namespace editor {

class EngineController;

class TreeWidgetBuilderHelper {
public:
    explicit TreeWidgetBuilderHelper(const std::shared_ptr<EngineController>& engine_controller);

    void subscribeOnActiveNode(QWidget* widget, const std::shared_ptr<engine::Node>& node);
    void subscribeOnActiveComponent(QWidget* widget, const std::shared_ptr<engine::Component>& component);

private:
    void subscribeOnActiveSceneItem(QWidget* widget, const std::function<bool()> updater);

    std::shared_ptr<EngineController> m_engine_controller;
};

}
