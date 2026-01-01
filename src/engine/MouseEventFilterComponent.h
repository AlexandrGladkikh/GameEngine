#pragma once

#include "Component.h"

#include <cstdint>
#include <memory>

namespace engine {

class TransformComponent;
class MaterialComponent;
class NodePositioningHelper;

class MouseEventFilterComponent final : public Component {
public:
    explicit MouseEventFilterComponent(uint32_t id, const std::string& name, uint32_t owner_node, uint32_t owner_scene);
    ~MouseEventFilterComponent() override = default;

    void init() override;

    void update(uint64_t dt) override;

    [[nodiscard]]
    bool isDirty() const override;
    void markDirty() override;
    void clearDirty() override;

    [[nodiscard]]
    std::string type() const override;

    auto clone(uint32_t owner_node_id) const -> std::unique_ptr<Component> override;

    int key() const;
    int action() const;

    void setKey(int key);
    void setAction(int action);

private:
    std::shared_ptr<TransformComponent> m_transform;
    std::shared_ptr<MaterialComponent> m_material;
    std::shared_ptr<NodePositioningHelper> m_node_positioning_helper;

    int32_t mouse_cursor_x_pos = 0;
    int32_t mouse_cursor_y_pos = 0;

    bool m_update_mouse_position = false;

    int m_key = 0;
    int m_action = 0;
};

}
