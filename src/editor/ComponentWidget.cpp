#include "ComponentWidget.h"

#include <engine/Component.h>

#include <QApplication>
#include <QMouseEvent>
#include <QDrag>
#include <QMimeData>

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

void ComponentWidget::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        m_drag_start_pos = event->pos();
    }
}

void ComponentWidget::mouseMoveEvent(QMouseEvent* event)
{
    if (!(event->buttons() & Qt::LeftButton)) {
        return;
    }

    if ((event->pos() - m_drag_start_pos).manhattanLength() < QApplication::startDragDistance()) {
        return;
    }

    QDrag* drag = new QDrag(this);
    QMimeData* mime = new QMimeData;
    mime->setText(QString::number(m_component->id()));
    drag->setMimeData(mime);

    drag->exec(Qt::CopyAction | Qt::MoveAction);
}

}
