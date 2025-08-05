#include "Scene.h"
#include "Component.h"
#include "FileSystem.h"
#include "Node.h"
#include "ComponentBuilder.h"
#include "Logger.h"
#include "Context.h"
#include "UserComponentsBuilder.h"

#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include <ranges>

namespace engine {

Scene::Scene(const std::shared_ptr<Context>& context, uint32_t id, std::string name) :
    m_context(context),
    m_id(id),
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

uint32_t Scene::id() const
{
    return m_id;
}

std::string Scene::name() const
{
    return m_name;
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

auto Scene::getComponent(uint32_t id) const -> std::optional<std::shared_ptr<Component>>
{
    auto it = m_components.find(id);
    if (it == m_components.end()) {
        return std::nullopt;
    }

    return it->second;
}

auto Scene::getNode(uint32_t id) const -> std::optional<std::shared_ptr<Node>>
{
    auto it = m_nodes.find(id);
    if (it == m_nodes.end()) {
        return std::nullopt;
    }

    return it->second;
}

auto Scene::getComponents() const -> const std::unordered_map<uint32_t, std::shared_ptr<Component>>&
{
    return m_components;
}

auto Scene::getNodes() const -> const std::unordered_map<uint32_t, std::shared_ptr<Node>>&
{
    return m_nodes;
}

auto Scene::getRoot() const -> std::optional<std::shared_ptr<Node>>
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

auto Scene::getResources() const -> const std::vector<uint32_t>&
{
    return m_resources_id;
}

void Scene::addResource(uint32_t id)
{
    m_resources_id.push_back(id);
}

auto saveScene(const std::shared_ptr<Scene>& scene, const std::filesystem::path& path) -> bool
{
    auto file = FileSystem::file(path, std::ios::out);

    rapidjson::Document document;
    document.SetObject();
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    document.Accept(writer);

    rapidjson::Value value;
    value.SetString(scene->name().c_str(), document.GetAllocator());
    document.AddMember("id", scene->id(), document.GetAllocator());
    document.AddMember("name", value, document.GetAllocator());

    document.AddMember("root", scene->getRoot().value()->id(), document.GetAllocator());

    rapidjson::Value nodes(rapidjson::kArrayType);
    for (const auto& node : scene->getNodes()) {
        rapidjson::Value node_json(rapidjson::kObjectType);
        saveNode(node.second, node_json, document.GetAllocator());
        nodes.PushBack(node_json, document.GetAllocator());
    }
    document.AddMember("nodes", nodes, document.GetAllocator());

    rapidjson::Value components(rapidjson::kArrayType);
    for (const auto& component : scene->getComponents()) {
        rapidjson::Value component_json(rapidjson::kObjectType);
        ComponentBuilder::save(component.second, component_json, document.GetAllocator());
        components.PushBack(component_json, document.GetAllocator());
    }
    document.AddMember("components", components, document.GetAllocator());

    rapidjson::Value resources(rapidjson::kArrayType);
    for (const auto id : scene->getResources()) {
        resources.PushBack(id, document.GetAllocator());
    }
    document.AddMember("resources", resources, document.GetAllocator());

    return file.writeText(buffer.GetString());
}

auto buildScene(const std::shared_ptr<Context>& context, const std::filesystem::path &path) -> std::optional<std::unique_ptr<Scene>>
{
    Logger::info(__FUNCTION__);

    if (!FileSystem::exists(path) || !FileSystem::isFile(path)) {
        Logger::error("buildScene() File not found: {}", path.string());
        return std::nullopt;
    }

    auto file = FileSystem::file(path, std::ios::in | std::ios::binary);
    auto text = file.readText();

    rapidjson::Document document;
    document.Parse(text.c_str());

    auto id = document["id"].GetUint();
    auto name = document["name"].GetString();

    auto scene = std::make_unique<Scene>(context, id, name);

    auto root = document["root"].GetUint();
    scene->setRoot(root);

    auto nodes_json = document["nodes"].GetArray();
    for (auto& node_json : nodes_json) {
        auto node = buildNode(node_json);
        if (!node.has_value()) {
            continue;
        }

        node.value()->setContext(context);
        auto id = node.value()->id();

        scene->addNode(id, std::move(node.value()));
    }

    auto components_json = document["components"].GetArray();
    for (auto& component_json : components_json) {
        auto type = component_json["type"].GetString();
        auto component = ComponentBuilder::build(type, component_json);
         if (!component.has_value()) {
             component = context->userComponentsBuilder->buildComponent(type, component_json);
             if (!component.has_value()) {
                 continue;
             }
        }

        component.value()->setContext(context);
        component.value()->init();

        Logger::info("add component type: {}", type);
        uint32_t id = component.value()->id();
        scene->addComponent(id, std::move(component.value()));
    }

    auto resources_json = document["resources"].GetArray();
    for (auto& resource_json : resources_json) {
        auto id = resource_json["id"].GetUint();

        Logger::info("add resource id: {}", id);
        scene->addResource(id);
    }

    return scene;
}


}