#pragma once

#include <memory>
#include <optional>

namespace engine {
class Component;
}

class QWidget;

namespace editor {

class UserComponentBuilder {
public:
    UserComponentBuilder() = default;
    virtual ~UserComponentBuilder() = default;

    [[nodiscard]]
    virtual std::optional<QWidget*> buildComponent(const std::shared_ptr<engine::Component>& component) const = 0;
};

}
