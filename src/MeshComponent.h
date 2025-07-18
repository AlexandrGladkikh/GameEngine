#pragma once

#include "Component.h"

#include <glad/glad.h>

#include <string>

namespace engine {

class MeshComponent final : public Component {
public:
    explicit MeshComponent(const std::string& meshId);

    ~MeshComponent() override = default;

    void bind() const;
    void unbind() const;

private:
    std::string m_id;
};

}
