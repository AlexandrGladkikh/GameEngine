#include "ResourcePackage.h"
#include "Context.h"
#include "MeshStore.h"
#include "ShaderStore.h"
#include "TextureStore.h"
#include "FileSystem.h"
#include "Shader.h"
#include "Texture.h"
#include "MeshBuilder.h"

#include <rapidjson/document.h>

namespace engine {

std::optional<ResourcePackage> buildResourcePackage(const std::filesystem::path& path)
{
    if (!exists(path) || (path.extension() != ".pkg" && path.extension() != ".json")) {
        return {};
    }

    ResourcePackage package;

    auto file = FileSystem::file(path, std::ios::in);
    auto text = file.readText();

    rapidjson::Document document;
    document.Parse(text.c_str());

    auto loadResourceInfo = [&document](const std::string& name, std::list<ResourceInfo>& resources) {
        if (document.HasMember(name.c_str())) {
            auto resources_json = document[name.c_str()].GetArray();
            for (auto resource_json : resources_json) {
                auto id = resource_json["id"].GetUint();
                auto path = resource_json["path"].GetString();
                auto resource = ResourceInfo{id, path};
                resources.push_back(resource);
            }
        }
    };

    loadResourceInfo("meshes", package.meshes);
    loadResourceInfo("shaders", package.shaders);
    loadResourceInfo("textures", package.textures);

    return package;
}

void saveResourcePackage(const std::filesystem::path& path, const ResourcePackage& package)
{
    rapidjson::Document document;
    document.SetObject();

    auto writeResourceInfo = [&document](const std::string& name, const std::list<ResourceInfo>& resources) {
        rapidjson::Value resources_json(rapidjson::kArrayType);
        for (const auto& resource : resources) {
            auto resource_json = rapidjson::Value(rapidjson::kObjectType);
            resource_json.AddMember("id", resource.id, document.GetAllocator());
            resource_json.AddMember("path", resource.path, document.GetAllocator());
            resources_json.PushBack(resource_json, document.GetAllocator());
        }

        rapidjson::Value key;
        key.SetString(name.c_str(), document.GetAllocator());

        document.AddMember(key, resources_json, document.GetAllocator());
    };

    writeResourceInfo("meshes", package.meshes);
    writeResourceInfo("shaders", package.shaders);
    writeResourceInfo("textures", package.textures);

    auto file = FileSystem::file(path, std::ios::out);
    file.writeText(document.GetString());
}

void loadResourcePackage(const std::shared_ptr<Context>& context, const ResourcePackage& package)
{
    for (auto& shaderInfo : package.shaders) {
        auto shader_existed = context->shaderStore->get(shaderInfo.id);
        if (shader_existed.has_value()) {
            continue;
        }

        auto new_shader = buildShader(shaderInfo.path);
        if (!new_shader.has_value()) {
            continue;
        }

        context->shaderStore->add(shaderInfo.id, std::move(new_shader.value()));
    }

    for (auto& textureInfo : package.textures) {
        auto texture_existed = context->textureStore->get(textureInfo.id);
        if (texture_existed.has_value()) {
            continue;
        }

        auto new_texture = buildTexture(textureInfo.path);
        if (!new_texture.has_value()) {
            continue;
        }

        context->textureStore->add(textureInfo.id, std::move(new_texture.value()));
    }

    for (auto& meshInfo : package.meshes) {
        auto mesh_existed = context->meshStore->get(meshInfo.id);
        if (mesh_existed.has_value()) {
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