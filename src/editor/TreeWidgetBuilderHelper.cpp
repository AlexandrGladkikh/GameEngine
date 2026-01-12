#include "TreeWidgetBuilderHelper.h"

#include "EngineController.h"

#include <engine/Component.h>
#include <engine/Node.h>

#include <QWidget>
#include <QGraphicsOpacityEffect>

namespace editor {

TreeWidgetBuilderHelper::TreeWidgetBuilderHelper(const std::shared_ptr<EngineController>& engine_controller) :
    m_engine_controller(engine_controller)
{
}

void TreeWidgetBuilderHelper::subscribeOnActiveNode(QWidget* widget, const std::shared_ptr<engine::Node>& node)
{
    subscribeOnActiveSceneItem(widget, [node]() {
        return node->isActive();
    });
}

void TreeWidgetBuilderHelper::subscribeOnActiveComponent(QWidget* widget, const std::shared_ptr<engine::Component>& component)
{
    subscribeOnActiveSceneItem(widget, [component]() {
        return component->isActive();
    });
}

void TreeWidgetBuilderHelper::subscribeOnActiveSceneItem(QWidget* widget, const std::function<bool()> updater)
{
    std::uintptr_t id = reinterpret_cast<std::uintptr_t>(widget);
    QGraphicsOpacityEffect* opacity_effect = new QGraphicsOpacityEffect(widget);
    widget->setGraphicsEffect(opacity_effect);
    m_engine_controller->addHandler(id, [updater, opacity_effect]() {
        opacity_effect->setOpacity(updater() ? 1.0f : 0.5f);
    });
    std::weak_ptr<EngineController> observer_weak = m_engine_controller;
    QObject::connect(widget, &QObject::destroyed, [observer_weak, id]() {
        if (auto observer = observer_weak.lock(); observer) {
            observer->removeHandler(id);
        }
    });
}

}
