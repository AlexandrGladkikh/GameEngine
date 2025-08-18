#include "ComponentWidget.h"

namespace editor {

ComponentWidget::ComponentWidget(QWidget* parent) :
    QWidget(parent)
{

}

void ComponentWidget::setComponent(const std::shared_ptr<engine::Component>& component)
{
    m_component = component;
}

std::shared_ptr<engine::Component> ComponentWidget::component() const
{
    return m_component;
}

}