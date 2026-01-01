#pragma once

#include <utility>
#include <memory>
#include <optional>

namespace engine {

class Node;
class TransformComponent;
class MaterialComponent;
struct Context;

class NodePositioningHelper final {
public:
    explicit NodePositioningHelper(const std::weak_ptr<Context>& context, const std::shared_ptr<Node>& node);
    std::optional<std::pair<float, float>> getAbsoluteNodePosition() const;

private:
    std::shared_ptr<Node> m_node;
    std::weak_ptr<Context> m_context;
    
    std::shared_ptr<TransformComponent> m_transform;
    std::shared_ptr<MaterialComponent> m_material;
};

}
