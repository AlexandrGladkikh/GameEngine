#pragma once

#include "Component.h"

#include <string>
#include <cstdint>
#include <unordered_map>
#include <any>

namespace engine {

class RenderScopeComponent final : public Component {
public:
    struct RenderData {
        std::unordered_map<std::string, std::any> uniforms{};
        bool is_sprite = false;
    };

    explicit RenderScopeComponent(uint32_t id, const std::string& name, uint32_t owner_node, uint32_t owner_scene);
    ~RenderScopeComponent() = default;

    void init() override;
    void update(uint64_t dt) override;

    [[nodiscard]]
    bool isDirty() const override;
    void markDirty() override;
    void clearDirty() override;

    auto type() const -> std::string override;

    auto clone(uint32_t owner_node_id) const -> std::unique_ptr<Component> override;
    
    [[nodiscard]]
    auto isSprite() const -> bool;
    void setIsSprite(bool is_sprite);

    [[nodiscard]]
    auto renderData() const -> const RenderData&;
    void setRenderData(const RenderData& render_data);

    [[nodiscard]]
    auto renderData(const std::string& name) const -> std::optional<std::any>;
    void addRenderData(const std::string& name, const std::any& value);

private:
    RenderData m_render_data;
};

}
