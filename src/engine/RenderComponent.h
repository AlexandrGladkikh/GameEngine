#pragma once

#include "Component.h"

#include <string>
#include <cstdint>
#include <unordered_map>

namespace engine {

class RenderComponent final : public Component {
public:
    struct RenderData {
        std::unordered_map<std::string, std::string> uniforms{};
    };

    explicit RenderComponent(uint32_t id, const std::string& name, uint32_t owner_node, uint32_t owner_scene);
    ~RenderComponent() = default;

    void init() override;
    void update(uint64_t dt) override;

    [[nodiscard]]
    bool isDirty() const override;
    void markDirty() override;
    void clearDirty() override;

    auto type() const -> std::string override;

    auto clone(uint32_t owner_node_id) const -> std::unique_ptr<Component> override;

    auto isSprite() const -> bool;
    void setSprite(bool sprite);

    [[nodiscard]]
    auto renderData() const -> const RenderData&;
    void setRenderData(const RenderData& render_data);

private:
    bool m_sprite = false;
    RenderData m_render_data;
};

}
