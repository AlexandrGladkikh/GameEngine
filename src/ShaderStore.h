#pragma once

#include <unordered_map>
#include <string>
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

    auto get(const std::string& id) const -> std::optional<std::shared_ptr<Shader>>;
    void add(const std::string& id, std::unique_ptr<Shader> shader);

private:
    std::unordered_map<std::string, std::shared_ptr<Shader>> m_shaders;
};

}
