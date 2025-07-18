#pragma once

#include <unordered_map>
#include <string>
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

    auto get(const std::string& id) const -> std::optional<std::shared_ptr<Texture>>;
    void add(const std::string& id, std::unique_ptr<Texture> texture);

private:
    std::unordered_map<std::string, std::shared_ptr<Texture>> m_textures;
};

}