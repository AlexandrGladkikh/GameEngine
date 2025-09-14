#pragma once

#include <cstdint>
#include <list>
#include <filesystem>
#include <memory>
#include <optional>
#include <string>

namespace engine {

struct Context;

struct ResourceInfo {
    uint32_t id;
    std::filesystem::path path;
};

struct ResourcePackage {
    std::string name;

    std::list<ResourceInfo> meshes;
    std::list<ResourceInfo> shaders;
    std::list<ResourceInfo> textures;
};

auto buildResourcePackage(const std::filesystem::path& path) -> std::optional<std::shared_ptr<ResourcePackage>>;
void saveResourcePackage(const std::shared_ptr<ResourcePackage>& package, uint32_t id, const std::filesystem::path& path);

void loadResourcePackage(const std::shared_ptr<Context>& context, const std::shared_ptr<ResourcePackage>& package);

}
