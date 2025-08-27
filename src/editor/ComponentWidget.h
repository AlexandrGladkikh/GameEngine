#pragma once

#include <QWidget>

#include <memory>

namespace engine {
class Component;
}

namespace editor {

class ComponentWidget final : public QWidget {
    Q_OBJECT

public:
    explicit ComponentWidget(QWidget* parent = nullptr);
    ~ComponentWidget() = default;

    void setComponent(const std::shared_ptr<engine::Component>& component);
    std::shared_ptr<engine::Component> component() const;

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;

private:
    std::shared_ptr<engine::Component> m_component;

    QPoint m_drag_start_pos;
};

}
