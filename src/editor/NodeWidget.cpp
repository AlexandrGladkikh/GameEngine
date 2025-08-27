#include "NodeWidget.h"

#include <engine/Node.h>

#include <QApplication>
#include <QMouseEvent>
#include <QDrag>
#include <QMimeData>

namespace editor {

NodeWidget::NodeWidget(QWidget* parent) :
    QWidget(parent)
{
    
}

void NodeWidget::setNode(const std::shared_ptr<engine::Node>& node)
{
    m_node = node;
}

auto NodeWidget::node() const -> std::shared_ptr<engine::Node>
{
    return m_node;
}

void NodeWidget::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        m_drag_start_pos = event->pos();
    }
}

void NodeWidget::mouseMoveEvent(QMouseEvent* event)
{
    if (!(event->buttons() & Qt::LeftButton)) {
        return;
    }

    if ((event->pos() - m_drag_start_pos).manhattanLength() < QApplication::startDragDistance()) {
        return;
    }

    QDrag* drag = new QDrag(this);
    QMimeData* mime = new QMimeData;
    mime->setText(QString::number(m_node->id()));
    drag->setMimeData(mime);

    drag->exec(Qt::CopyAction | Qt::MoveAction);
}

}
