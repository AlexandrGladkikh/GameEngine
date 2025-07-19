#include "Scene.h"
#include "Component.h"
#include "FileSystem.h"
#include "Node.h"
#include "ComponentBuilder.h"

#include <rapidjson/document.h>

#include <ranges>

namespace engine {

Scene::Scene(const std::shared_ptr<Context>& context, std::string id, std::string name) :
    m_context(context),
    m_id(std::move(id)),
    m_name(std::move(name))
{
}

void Scene::update(uint64_t dt)
{
    for (const auto &component : m_components | std::views::values) {
        if (component->isActive()) {
            component->update(dt);
        }

        if (component->isDirty()) {
            setDirty(true);
        }
    }
}

bool Scene::isActive() const
{
    return m_is_active;
}

bool Scene::isDirty() const
{
    return m_dirty;
}

void Scene::setActive(bool active)
{
    m_is_active = active;
}

void Scene::setDirty(bool dirty)
{
    m_dirty = dirty;
}

bool Scene::addComponent(uint32_t id, const std::shared_ptr<Component>& component)
{
    return m_components.insert({id, component}).second;
}

bool Scene::addNode(uint32_t id, const std::shared_ptr<Node>& node)
{
    return m_nodes.insert({id, node}).second;
}

bool Scene::removeComponent(uint32_t id)
{
    return m_components.erase(id) == 1;
}

bool Scene::removeNode(uint32_t id)
{
    auto it = m_nodes.find(id);
    if (it != m_nodes.end()) {
        auto node = it->second;
        m_nodes.erase(it);

        for (const auto component_id : node->components()) {
            removeComponent(component_id);
        }

        auto parent = getNode(node->parent());
        if (parent.has_value()) {
            parent.value()->removeChild(id);
        }

        for (const auto child_id : node->children()) {
            removeNode(child_id);
        }

        return true;
    }

    return false;
}

auto Scene::getComponent(uint32_t id) const
{
    auto it = m_components.find(id);
    if (it == m_components.end()) {
        return std::nullopt;
    }

    return it->second;
}

auto Scene::getNode(uint32_t id) const
{
    auto it = m_nodes.find(id);
    if (it == m_nodes.end()) {
        return std::nullopt;
    }

    return it->second;
}

auto Scene::getRoot() const
{
    auto it = m_nodes.find(m_root);
    if (it == m_nodes.end()) {
        return std::nullopt;
    }

    return it->second;
}

void Scene::setRoot(uint32_t id)
{
    m_root = id;
}

auto Scene::getResources() const
{
    return m_resources_id;
}

void Scene::addResource(uint32_t id)
{
    m_resources_id.push_back(id);
}

auto buildScene(std::shared_ptr<Context> context, const std::filesystem::path &path)
{
    if (!FileSystem::exists(path) || !FileSystem::isFile(path)) {
        return std::nullopt;
    }

    auto file = FileSystem::file(path, std::ios::in | std::ios::binary);
    auto text = file.readText();

    rapidjson::Document document;
    document.Parse(text.c_str());

    auto id = document["id"].GetString();
    auto name = document["name"].GetString();

    auto scene = std::make_unique<Scene>(context, id, name);

    auto root = document["root"].GetUint();
    scene->setRoot(root);

    auto nodes_json = document["nodes"].GetArray();
    for (auto node_json : nodes_json) {
        auto id = node_json["id"].GetUint();
        auto name = node_json["name"].GetString();
        auto parent = node_json["parent"].GetUint();
        auto node = std::make_shared<Node>(id, name, parent);
        scene->addNode(id, node);
    }

    auto components_json = document["components"].GetArray();
    for (auto component_json : components_json) {

        auto type = component_json["type"].GetString();
        auto component = ComponentBuilder::build(type, component_json);
         if (!component.has_value()) {
            continue;
        }

        scene->addComponent(component.value()->id(), std::move(component.value()));
    }

    auto resources_json = document["resources"].GetArray();
    for (auto resource_json : resources_json) {
        auto id = resource_json["id"].GetUint();
        scene->addResource(id);
    }

    return scene;
}


}