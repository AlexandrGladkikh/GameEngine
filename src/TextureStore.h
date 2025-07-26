#pragma once

#include <unordered_map>
#include <memory>
#include <optional>

namespace engine {

class Texture;

class TextureStore final {
public:
    TextureStore() = default;
    ~TextureStore() = default;
    TextureStore(const TextureStore&) = delete;
    TextureStore(TextureStore&&) = delete;
    TextureStore& operator=(const TextureStore&) = delete;
    TextureStore& operator=(TextureStore&&) = delete;

    auto get(uint32_t id) const -> std::optional<std::shared_ptr<Texture>>;
    void add(uint32_t id, std::unique_ptr<Texture> texture);

private:
    std::unordered_map<uint32_t, std::shared_ptr<Texture>> m_textures;
};

}