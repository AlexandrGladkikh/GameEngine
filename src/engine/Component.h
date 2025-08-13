#pragma once

#include <memory>
#include <string>
#include <optional>

namespace engine {

struct Context;
class Node;

class Component {
public:
    explicit Component(uint32_t id, const std::string& name, uint32_t owner_node, uint32_t owner_scene);
    virtual ~Component() = default;

    Component(const Component&) = delete;
    Component(Component&&) = delete;
    Component& operator=(const Component&) = delete;
    Component& operator=(Component&&) = delete;

    void setContext(const std::weak_ptr<Context>& context);
    [[nodiscard]]
    auto context() const -> std::weak_ptr<Context>;

    [[nodiscard]]
    bool isActive() const;
    void setActive(bool active);

    std::optional<std::shared_ptr<Node>> getNode() const;

    [[nodiscard]]
    uint32_t id() const;
    [[nodiscard]]
    const std::string& name() const;
    [[nodiscard]]
    uint32_t ownerNode() const;
    [[nodiscard]]
    uint32_t ownerScene() const;

    virtual void init() = 0;

    virtual void update(uint64_t dt) = 0;

    [[nodiscard]]
    virtual bool isDirty() const = 0;
    virtual void markDirty() = 0;
    virtual void clearDirty() = 0;

    [[nodiscard]]
    virtual std::string type() const = 0;

private:
    std::weak_ptr<Context> m_context;

    uint32_t m_id = 0;
    std::string m_name;
    uint32_t m_owner_node;
    uint32_t m_owner_scene;

    bool m_is_active = true;
};

}