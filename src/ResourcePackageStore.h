#pragma once

#include <unordered_map>
#include <optional>
#include <memory>
#include <string>

namespace engine {

class ResourcePackage;

class ResourcePackageStore {
public:
    ResourcePackageStore() = default;
    ~ResourcePackageStore() = default;
    ResourcePackageStore(const ResourcePackageStore&) = delete;
    ResourcePackageStore(ResourcePackageStore&&) = delete;
    ResourcePackageStore& operator=(const ResourcePackageStore&) = delete;
    ResourcePackageStore& operator=(ResourcePackageStore&&) = delete;

    auto get(const std::string& id) const -> std::optional<std::shared_ptr<ResourcePackage>>;
    void add(const std::string& id, const std::shared_ptr<ResourcePackage>& resourcePackage);

private:
    std::unordered_map<std::string, std::shared_ptr<ResourcePackage>> m_resourcePackages;
};

}