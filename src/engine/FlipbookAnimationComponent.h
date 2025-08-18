#pragma once

#include "Component.h"

#include <vector>

namespace engine {

class FlipbookAnimationComponent final : public Component {
public:
    explicit FlipbookAnimationComponent(uint32_t id, const std::string& name, uint32_t owner_node, uint32_t owner_scene);
    ~FlipbookAnimationComponent() override = default;

    FlipbookAnimationComponent(const FlipbookAnimationComponent&) = delete;
    FlipbookAnimationComponent(FlipbookAnimationComponent&&) = delete;
    FlipbookAnimationComponent& operator=(const FlipbookAnimationComponent&) = delete;
    FlipbookAnimationComponent& operator=(FlipbookAnimationComponent&&) = delete;

    void init() override;
    void update(uint64_t dt) override;
    [[nodiscard]]
    bool isDirty() const override;
    void markDirty() override;
    void clearDirty() override;
    std::string type() const override;

    void start();
    void stop();

    bool isRunning() const;

    void addMaterial(uint32_t material_id);
    void addMaterial(const std::string& material_name);

    auto materialIds() const -> const std::vector<uint32_t>&;
    void removeMaterial(uint32_t material_id);

    void setUpdateTime(uint64_t update_time);
    auto updateTime() const -> uint64_t;

private:
    std::vector<uint32_t> m_material_ids;

    uint64_t m_update_time = 0;

    bool m_run = false;
    uint64_t m_from_last_update = 0;

    size_t m_current_material = 0;
};

}
