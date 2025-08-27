#include "TreeWidgetBuilderHelper.h"

#include "EngineObserver.h"

#include <engine/Node.h>
#include <engine/Component.h>

#include <QWidget>
#include <QGraphicsOpacityEffect>

namespace editor {

TreeWidgetBuilderHelper::TreeWidgetBuilderHelper(const std::shared_ptr<EngineObserver>& engine_observer) :
    m_engine_observer(engine_observer)
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
    m_engine_observer->addHandler(id, [updater, opacity_effect]() {
        opacity_effect->setOpacity(updater() ? 1.0f : 0.5f);
    });
    QObject::connect(widget, &QObject::destroyed, [this, id]() {
        m_engine_observer->removeHandler(id);
    });
}

}
