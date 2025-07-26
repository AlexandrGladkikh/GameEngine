#pragma once

#include "Component.h"

namespace engine {

class MeshComponent final : public Component {
public:
    explicit MeshComponent(uint32_t id, const std::string& name, uint32_t owner,
        uint32_t meshId);

    ~MeshComponent() override = default;

    void update(uint64_t dt) override;

    [[nodiscard]]
    bool isDirty() const override;
    void markDirty() override;
    void clearDirty() override;

    std::string type() const override;

    void bind() const;
    void unbind() const;

    uint32_t meshId() const;

private:
    uint32_t m_id;

    bool m_dirty;
};

}
