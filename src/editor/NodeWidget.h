#pragma once

#include <QWidget>
#include <QPoint>

#include <memory>

class QMouseEvent;

namespace engine {
class Node;
}

namespace editor {

class NodeWidget final : public QWidget {
    Q_OBJECT
    
public:
    explicit NodeWidget(QWidget* parent = nullptr);
    ~NodeWidget() = default;

    void setNode(const std::shared_ptr<engine::Node>& node);
    auto node() const -> std::shared_ptr<engine::Node>;

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;

private:
    std::shared_ptr<engine::Node> m_node;

    QPoint m_drag_start_pos;
};

}
