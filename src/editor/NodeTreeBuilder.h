#pragma once

#include <memory>
#include <optional>

class QWidget;

namespace engine {
class Component;
}

namespace editor {

class NodeWidget;
class ComponentWidget;

class NodeTreeBuilder final {
public:
    static auto buildWidgetForNode(const std::string& node_name) -> std::optional<NodeWidget*>;
    static auto buildWidgetForComponent(std::shared_ptr<engine::Component> component) -> std::optional<ComponentWidget*>;
};

}
