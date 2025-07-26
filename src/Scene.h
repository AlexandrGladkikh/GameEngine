#pragma once

#include <memory>
#include <unordered_map>
#include <string>
#include <vector>
#include <optional>
#include <filesystem>

namespace engine {

struct Context;
class Component;
class Node;

class Scene {
public:
    explicit Scene(const std::shared_ptr<Context>& context, uint32_t id, std::string name);

    void update(uint64_t dt);

    uint32_t id() const;
    auto name() const -> std::string;

    bool isActive() const;
    bool isDirty() const;

    void setActive(bool active);
    void setDirty(bool dirty);

    bool addComponent(uint32_t id, const std::shared_ptr<Component>& component);
    bool addNode(uint32_t id, const std::shared_ptr<Node>& node);

    bool removeComponent(uint32_t id);
    bool removeNode(uint32_t id);

    auto getComponent(uint32_t id) const -> std::optional<std::shared_ptr<Component>>;
    auto getNode(uint32_t id) const -> std::optional<std::shared_ptr<Node>>;

    auto getComponents() const -> const std::unordered_map<uint32_t, std::shared_ptr<Component>>&;
    auto getNodes() const -> const std::unordered_map<uint32_t, std::shared_ptr<Node>>&;

    auto getRoot() const -> std::optional<std::shared_ptr<Node>>;

    void setRoot(uint32_t id);

    auto getResources() const -> const std::vector<uint32_t>&;
    void addResource(uint32_t id);

private:
    uint32_t m_id;
    std::string m_name;

    bool m_is_active = false;

    bool m_dirty = false;

    std::weak_ptr<Context> m_context;
    uint32_t m_root;

    std::unordered_map<uint32_t, std::shared_ptr<Component>> m_components;
    std::unordered_map<uint32_t, std::shared_ptr<Node>> m_nodes;

    std::vector<uint32_t> m_resources_id;
};

auto saveScene(const std::shared_ptr<Scene>& scene, const std::filesystem::path& path) -> bool;

auto buildScene(const std::shared_ptr<Context>& context, const std::filesystem::path& path) -> std::optional<std::unique_ptr<Scene>>;

}
