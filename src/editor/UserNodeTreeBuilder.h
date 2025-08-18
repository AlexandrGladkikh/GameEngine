#pragma once

#include <memory>
#include <optional>

namespace engine {
class Component;
}

class QWidget;

namespace editor {

class ComponentWidget;

class UserNodeTreeBuilder {
public:
    UserNodeTreeBuilder() = default;
    virtual ~UserNodeTreeBuilder() = default;

    [[nodiscard]]
    virtual std::optional<ComponentWidget*> buildWidgetForComponent(const std::shared_ptr<engine::Component>& component) const = 0;
};

}
