#pragma once

#include "Component.h"

namespace engine {

class MeshComponent final : public Component {
public:
    explicit MeshComponent(uint32_t id, const std::string& name, uint32_t owner_node, uint32_t owner_scene);

    ~MeshComponent() override = default;

    void init() override;

    void update(uint64_t dt) override;

    [[nodiscard]]
    bool isDirty() const override;
    void markDirty() override;
    void clearDirty() override;

    std::string type() const override;

    void bind() const;
    void unbind() const;

    void setMesh(uint32_t meshId);
    void setMesh(const std::string& mesh_name);

    uint32_t meshId() const;

private:
    uint32_t m_id = 0;

    bool m_dirty;
};

}
