#include "Scene.h"
#include "Component.h"
#include "FileSystem.h"
#include "Node.h"
#include "ComponentBuilder.h"
#include "Logger.h"
#include "Context.h"
#include "UserComponentsBuilder.h"
#include "Utils.h"
#include "SceneConfig.h"

#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>

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

void Scene::setName(std::string name)
{
    if (m_name == name) {
        return;
    }
    m_name = std::move(name);
    setDirty(true);
}

auto Scene::context() const -> std::shared_ptr<Context>
{
    return m_context.lock();
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

auto Scene::createRootNode(const std::string& name) -> std::optional<std::shared_ptr<Node>>
{
    if (!m_nodes.empty()) {
        return std::nullopt;
    }

    auto id = generateUniqueId();
    auto root_node = std::make_shared<Node>(id, name, 0, m_id);
    root_node->setContext(m_context);
    m_nodes.insert({id, root_node});
    m_root = id;

    return root_node;
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

        auto parent = getNode(node->getParentId());
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

auto Scene::getComponent(const std::string& name) const -> std::optional<std::shared_ptr<Component>>
{
    auto component = std::ranges::find_if(m_components, [&](const auto& component) {
        return component.second->name() == name;
    });

    if (component == m_components.end()) {
        return std::nullopt;
    }
    return component->second;
}

auto Scene::getNode(uint32_t id) const -> std::optional<std::shared_ptr<Node>>
{
    auto it = m_nodes.find(id);
    if (it == m_nodes.end()) {
        return std::nullopt;
    }

    return it->second;
}

auto Scene::getNode(const std::string& name) const -> std::optional<std::shared_ptr<Node>>
{
    auto node = std::ranges::find_if(m_nodes, [&](const auto& node) {
        return node.second->name() == name;
    });

    if (node == m_nodes.end()) {
        return std::nullopt;
    }
    return node->second;
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
    if (std::ranges::find(m_resources_id, id) != m_resources_id.end()) {
        return;
    }
    m_resources_id.push_back(id);
    setDirty(true);
}

void Scene::setResources(std::vector<uint32_t> ids)
{
    std::vector<uint32_t> unique;
    unique.reserve(ids.size());
    for (auto id : ids) {
        if (std::ranges::find(unique, id) == unique.end()) {
            unique.push_back(id);
        }
    }

    if (m_resources_id == unique) {
        return;
    }

    m_resources_id = std::move(unique);
    setDirty(true);
}

auto saveSceneToFile(const std::shared_ptr<Scene>& scene, const std::filesystem::path& path) -> bool
{
    if (FileSystem::exists(path)) {
        FileSystem::removeFile(path);
    }

    auto file = FileSystem::file(path, std::ios::out);

    rapidjson::Document document;
    document.SetObject();

    rapidjson::Value value;
    value.SetString(scene->name().c_str(), document.GetAllocator());
    document.AddMember("id", scene->id(), document.GetAllocator());
    document.AddMember("name", value, document.GetAllocator());

    auto root_node = scene->getRoot();
    uint32_t root_id = -1;
    if (root_node.has_value()) {
        root_id = root_node.value()->id();
    }

    document.AddMember("root", root_id, document.GetAllocator());

    rapidjson::Value nodes(rapidjson::kArrayType);
    for (const auto& node : scene->getNodes()) {
        rapidjson::Value node_json(rapidjson::kObjectType);
        saveNode(node.second, node_json, document.GetAllocator());
        nodes.PushBack(node_json, document.GetAllocator());
    }
    document.AddMember("nodes", nodes, document.GetAllocator());

    auto ctx = scene->context();
    rapidjson::Value components(rapidjson::kArrayType);
    for (const auto& component : scene->getComponents()) {
        rapidjson::Value component_json(rapidjson::kObjectType);
        ComponentBuilder::saveToJson(component.second, component_json, document.GetAllocator());
        if (component_json.ObjectEmpty()) {
            ctx->userComponentsBuilder->saveToJson(component.second, component_json, document.GetAllocator());
        }
        components.PushBack(component_json, document.GetAllocator());
    }
    document.AddMember("components", components, document.GetAllocator());

    rapidjson::Value resources(rapidjson::kArrayType);
    for (const auto id : scene->getResources()) {
        rapidjson::Value resource_json(rapidjson::kObjectType);
        resource_json.AddMember("id", id, document.GetAllocator());
        resources.PushBack(resource_json, document.GetAllocator());
    }
    document.AddMember("resources", resources, document.GetAllocator());

    rapidjson::StringBuffer buffer;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
    document.Accept(writer);

    Logger::debug("Saving scene: {}", path.string());
    Logger::debug("Scene data: {}", buffer.GetString());

    return file.writeText(buffer.GetString());
}

auto buildScene(const std::shared_ptr<Context>& context, const std::shared_ptr<SceneConfig>& scene_config) -> std::optional<std::unique_ptr<Scene>>
{
    Logger::debug(__FUNCTION__);

    const auto& document = scene_config->document();
    auto id = scene_config->id();
    auto name = scene_config->name();

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
        auto component = ComponentBuilder::buildFromJson(type, component_json);
         if (!component.has_value()) {
             component = context->userComponentsBuilder->buildComponent(type, component_json);
             if (!component.has_value()) {
                 continue;
             }
        }

        component.value()->setContext(context);

        Logger::debug("add component type: {}", type);
        uint32_t id = component.value()->id();
        scene->addComponent(id, std::move(component.value()));
    }

    auto resources_json = document["resources"].GetArray();
    for (auto& resource_json : resources_json) {
        auto id = resource_json["id"].GetUint();

        Logger::debug("add resource id: {}", id);
        scene->addResource(id);
    }

    return scene;
}

}
