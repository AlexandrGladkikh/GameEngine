#pragma once

#include <unordered_map>
#include <optional>
#include <memory>

namespace engine {

class Shader;

class ShaderStore {
public:
    ShaderStore() = default;
    ~ShaderStore() = default;

    ShaderStore(const ShaderStore&) = delete;
    ShaderStore& operator=(const ShaderStore&) = delete;

    ShaderStore(ShaderStore&&) = delete;
    ShaderStore& operator=(ShaderStore&&) = delete;

    auto get(uint32_t id) const -> std::optional<std::shared_ptr<Shader>>;
    void add(uint32_t id, std::unique_ptr<Shader> shader);

private:
    std::unordered_map<uint32_t, std::shared_ptr<Shader>> m_shaders;
};

}
