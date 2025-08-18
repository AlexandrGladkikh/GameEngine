#pragma once

#include <QWidget>

#include <memory>

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

private:
    std::shared_ptr<engine::Node> m_node;
};

}
