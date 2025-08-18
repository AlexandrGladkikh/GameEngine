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

private:
    std::shared_ptr<engine::Component> m_component;
};

}
