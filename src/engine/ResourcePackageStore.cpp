#include "ResourcePackageStore.h"

#include "FileSystem.h"

#include <rapidjson/document.h>

namespace engine {

auto ResourcePackageStore::get(uint32_t id) const -> std::optional<std::shared_ptr<ResourcePackage>>
{
    return m_resourcePackages.at(id);
}

void ResourcePackageStore::add(uint32_t id, const std::shared_ptr<ResourcePackage> &resourcePackage)
{
    m_resourcePackages[id] = resourcePackage;
}

void ResourcePackageStore::remove(uint32_t id)
{
    m_resourcePackages.erase(id);
}

auto ResourcePackageStore::getResourcePackagesInformation() const -> const std::unordered_map<uint32_t, std::filesystem::path>&
{
    return m_resourcePackagesInformation;
}

auto ResourcePackageStore::getResourcePackageInformation(uint32_t id) const -> std::optional<std::filesystem::path>
{
    if (!m_resourcePackagesInformation.contains(id)) {
        return std::nullopt;
    }

    return m_resourcePackagesInformation.at(id);
}

void ResourcePackageStore::initResourcePackagesInformation(const std::filesystem::path& location)
{
    if (!FileSystem::exists(location) || !FileSystem::isFile(location)) {
        return;
    }
    auto file = FileSystem::file(location, std::ios::in);
    auto text = file.readText();

    rapidjson::Document document;
    document.Parse(text.c_str());

    auto packages_json = document["packages"].GetArray();
    for (const auto& package_json : packages_json) {
        auto package_path = package_json.GetString();
        FileSystem::File package_file(package_path, std::ios::in);
        auto package_text = package_file.readText();
        rapidjson::Document package_document;
        package_document.Parse(package_text.c_str());
        auto package_id = package_document["id"].GetUint();
        m_resourcePackagesInformation[package_id] = package_path;
    }
}

}
