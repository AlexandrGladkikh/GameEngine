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

std::optional<ResourcePackage> buildResourcePackage(const std::filesystem::path& path) {
    if (!exists(path) || (path.extension() != ".pkg" && path.extension() != ".json")) {
        return {};
    }

    ResourcePackage package;

    auto file = FileSystem::file(path, std::ios::out);
    auto text = file.readText();

    rapidjson::Document document;
    document.Parse(text.c_str());

    auto loadResourceInfo = [&document](const std::string& name, std::list<ResourceInfo>& resources) {
        if (document.HasMember(name.c_str())) {
            auto resources_json = document[name.c_str()].GetArray();
            for (auto resource_json : resources_json) {
                auto id = resource_json["id"].GetString();
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

void saveResourcePackage(const std::filesystem::path& path, const ResourcePackage& package) {

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