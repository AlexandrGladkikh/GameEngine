#include "ResourcePackage.h"
#include "Context.h"
#include "MeshStore.h"
#include "ShaderStore.h"
#include "TextureStore.h"
#include "FileSystem.h"
#include "Shader.h"
#include "Texture.h"
#include "MeshBuilder.h"
#include "Logger.h"

#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>

namespace engine {

std::optional<std::shared_ptr<ResourcePackage>> buildResourcePackage(const std::filesystem::path& path)
{
    Logger::info(__FUNCTION__);

    if (!FileSystem::exists(path) || !FileSystem::isFile(path) || (path.extension() != ".pkg" && path.extension() != ".json")) {
        return {};
    }

    std::shared_ptr<ResourcePackage> package = std::make_shared<ResourcePackage>();

    auto file = FileSystem::file(path, std::ios::in);
    auto text = file.readText();

    rapidjson::Document document;
    document.Parse(text.c_str());

    package->name = document["name"].GetString();

    auto loadResourceInfo = [&document](const std::string& name, std::list<ResourceInfo>& resources) {
        if (document.HasMember(name.c_str())) {
            auto resources_json = document[name.c_str()].GetArray();
            for (auto& resource_json : resources_json) {
                auto id = resource_json["id"].GetUint();
                auto path = resource_json["path"].GetString();
                auto resource = ResourceInfo{id, path};
                resources.push_back(resource);
            }
        }
    };

    loadResourceInfo("meshes", package->meshes);
    loadResourceInfo("shaders", package->shaders);
    loadResourceInfo("textures", package->textures);

    return package;
}

void saveResourcePackage(const std::shared_ptr<ResourcePackage>& package, uint32_t id, const std::filesystem::path& path)
{
    rapidjson::Document document;
    document.SetObject();

    auto writeResourceInfo = [&document](const std::string& name, const std::list<ResourceInfo>& resources) {
        rapidjson::Value resources_json(rapidjson::kArrayType);
        for (const auto& resource : resources) {
            auto resource_json = rapidjson::Value(rapidjson::kObjectType);

            rapidjson::Value value;
            value.SetString(resource.path.string().c_str(), document.GetAllocator());
            resource_json.AddMember("id", resource.id, document.GetAllocator());
            resource_json.AddMember("path", value, document.GetAllocator());
            resources_json.PushBack(resource_json, document.GetAllocator());
        }

        rapidjson::Value key;
        key.SetString(name.c_str(), document.GetAllocator());

        document.AddMember(key, resources_json, document.GetAllocator());
    };

    rapidjson::Value value;
    value.SetString(package->name.c_str(), document.GetAllocator());
    document.AddMember("id", id, document.GetAllocator());
    document.AddMember("name", value, document.GetAllocator());

    writeResourceInfo("meshes", package->meshes);
    writeResourceInfo("shaders", package->shaders);
    writeResourceInfo("textures", package->textures);

    rapidjson::StringBuffer buffer;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
    document.Accept(writer);

    auto file = FileSystem::file(path, std::ios::out);
    file.writeText(buffer.GetString());
}

void loadResourcePackage(const std::shared_ptr<Context>& context, const std::shared_ptr<ResourcePackage>& package)
{
    Logger::info(__FUNCTION__);

    for (auto& shaderInfo : package->shaders) {
        auto shader_exist = context->shaderStore->get(shaderInfo.id);
        if (shader_exist.has_value()) {
            continue;
        }

        auto new_shader = buildShader(shaderInfo.path);
        if (!new_shader.has_value()) {
            continue;
        }

        context->shaderStore->add(shaderInfo.id, std::move(new_shader.value()));
    }

    for (auto& textureInfo : package->textures) {
        auto texture_exist = context->textureStore->get(textureInfo.id);
        if (texture_exist.has_value()) {
            continue;
        }

        auto new_texture = buildTexture(textureInfo.path);
        if (!new_texture.has_value()) {
            continue;
        }

        context->textureStore->add(textureInfo.id, std::move(new_texture.value()));
    }

    for (auto& meshInfo : package->meshes) {
        auto mesh_exist = context->meshStore->get(meshInfo.id);
        if (mesh_exist.has_value()) {
            continue;
        }

        auto new_mesh = buildMesh(meshInfo.path);
        if (!new_mesh.has_value()) {
            continue;
        }

        context->meshStore->add(meshInfo.id, std::move(new_mesh.value()));
    }
}

}
