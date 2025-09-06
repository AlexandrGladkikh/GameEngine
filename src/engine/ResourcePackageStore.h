#pragma once

#include <cstdint>
#include <unordered_map>
#include <optional>
#include <memory>
#include <filesystem>

namespace engine {

class ResourcePackage;

class ResourcePackageStore final {
public:
    ResourcePackageStore() = default;
    ~ResourcePackageStore() = default;
    ResourcePackageStore(const ResourcePackageStore&) = delete;
    ResourcePackageStore(ResourcePackageStore&&) = delete;
    ResourcePackageStore& operator=(const ResourcePackageStore&) = delete;
    ResourcePackageStore& operator=(ResourcePackageStore&&) = delete;

    auto get(uint32_t id) const -> std::optional<std::shared_ptr<ResourcePackage>>;
    void add(uint32_t id, const std::shared_ptr<ResourcePackage>& resourcePackage);
    bool contains(uint32_t id);
    void remove(uint32_t id);

    auto getResourcePackages() const -> const std::unordered_map<uint32_t, std::shared_ptr<ResourcePackage>>&;

    auto getResourcePackagesInformation() const -> const std::unordered_map<uint32_t, std::filesystem::path>&;
    auto getResourcePackageInformation(uint32_t id) const -> std::optional<std::filesystem::path>;

    void initResourcePackagesInformation(const std::filesystem::path& location);

private:
    std::unordered_map<uint32_t, std::shared_ptr<ResourcePackage>> m_resourcePackages;

    std::unordered_map<uint32_t, std::filesystem::path> m_resourcePackagesInformation;
};

}
