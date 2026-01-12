#pragma once

#include <unordered_map>
#include <optional>
#include <memory>
#include <string>
#include <vector>

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
    auto get(const std::string& name) const -> std::optional<std::shared_ptr<Shader>>;
    auto getIdByName(const std::string& name) const -> std::optional<uint32_t>;
    void add(uint32_t id, std::unique_ptr<Shader> shader);
    void remove(uint32_t id);
    bool contains(uint32_t id) const;
    auto names() const -> std::vector<std::string>;

private:
    std::unordered_map<uint32_t, std::shared_ptr<Shader>> m_shaders;
};

}
