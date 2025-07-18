#pragma once

#include <list>
#include <string>
#include <filesystem>
#include <memory>
#include <optional>

namespace engine {

struct Context;

struct ResourceInfo {
    std::string id;
    std::filesystem::path path;
};

struct ResourcePackage {
    std::list<ResourceInfo> meshes;
    std::list<ResourceInfo> shaders;
    std::list<ResourceInfo> textures;
};

auto buildResourcePackage(const std::filesystem::path& path) -> std::optional<ResourcePackage>;
void saveResourcePackage(const ResourcePackage& package, const std::filesystem::path& path);

void loadResourcePackage(const std::shared_ptr<Context>& context, const ResourcePackage& package);

}