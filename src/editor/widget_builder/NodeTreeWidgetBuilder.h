#pragma once

#include <memory>
#include <optional>

class QWidget;
class QTreeWidget;
class QTreeWidgetItem;

namespace engine {
class Component;
}

namespace editor {

class NodeWidget;
class ComponentWidget;

class NodeTreeWidgetBuilder final {
public:
    static auto buildWidgetForNode(const std::string& node_name) -> std::optional<NodeWidget*>;
    static auto buildWidgetForComponent(std::shared_ptr<engine::Component> component, QTreeWidget* tree, QTreeWidgetItem* item) -> std::optional<ComponentWidget*>;
};

}
