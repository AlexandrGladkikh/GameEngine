#pragma once

#include <memory>

class QWidget;

namespace engine {
class Component;
}

namespace editor {

class NodeBuilder final {
public:
    static QWidget* buildComponent(std::shared_ptr<engine::Component> component);
};

}
