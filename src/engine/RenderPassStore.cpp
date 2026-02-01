#include "RenderPassStore.h"

#include "renderpasses/BaseRenderPass.h"
#include "renderpasses/BaseLightRenderPass.h"

#include <ranges>

namespace engine {

RenderPassStore::RenderPassStore()
{
    m_renderPasses["base_render_pass"] = std::make_shared<BaseRenderPass>();
    m_renderPasses["base_light_render_pass"] = std::make_shared<BaseLightRenderPass>();
}

bool RenderPassStore::contains(std::string name) const
{
    return m_renderPasses.contains(name);
}

auto RenderPassStore::names() const -> std::vector<std::string>
{
    return m_renderPasses | std::views::keys | std::ranges::to<std::vector<std::string>>();
}

auto RenderPassStore::getAll() const -> std::unordered_map<std::string, std::shared_ptr<RenderPass>>
{
    return m_renderPasses;
}

auto RenderPassStore::get(std::string name) const -> std::optional<std::shared_ptr<RenderPass>>
{
    auto it = m_renderPasses.find(name);
    if (it == m_renderPasses.end()) {
        return std::nullopt;
    }
    return std::make_optional(it->second);
}

}