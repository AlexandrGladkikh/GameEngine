#include "NodeWidget.h"

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

}