#pragma once

#include "Component.h"

namespace engine {

class RenderPassComponent final : public Component {
public:
    explicit RenderPassComponent(uint32_t id, const std::string& name, uint32_t owner_node, uint32_t owner_scene);
    ~RenderPassComponent() = default;

    void init() override;
    void update(uint64_t dt) override;

    [[nodiscard]]
    bool isDirty() const override;
    void markDirty() override;
    void clearDirty() override;

    [[nodiscard]]
    std::string type() const override;

    auto clone(uint32_t owner_node_id) const -> std::unique_ptr<Component> override;

    void setRenderPassName(const std::string& render_pass_name);

    [[nodiscard]]
    auto renderPassName() const -> const std::string&;

private:
    std::string m_render_pass_name;
};

}