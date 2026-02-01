#pragma once

#include <unordered_map>
#include <optional>
#include <memory>
#include <string>

namespace engine {

class RenderPass;

class RenderPassStore final {
public:
    explicit RenderPassStore();
    ~RenderPassStore() = default;
    RenderPassStore(const RenderPassStore&) = delete;
    RenderPassStore(RenderPassStore&&) = delete;
    RenderPassStore& operator=(const RenderPassStore&) = delete;
    RenderPassStore& operator=(RenderPassStore&&) = delete;

    bool contains(std::string name) const;
    auto names() const -> std::vector<std::string>;
    auto getAll() const -> std::unordered_map<std::string, std::shared_ptr<RenderPass>>;
    auto get(std::string name) const -> std::optional<std::shared_ptr<RenderPass>>;

private:
    std::unordered_map<std::string, std::shared_ptr<RenderPass>> m_renderPasses;
};

}