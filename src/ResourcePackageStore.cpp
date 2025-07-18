#include "ResourcePackageStore.h"

namespace engine {

auto ResourcePackageStore::get(const std::string &id) const -> std::optional<std::shared_ptr<ResourcePackage>>
{
    return m_resourcePackages.at(id);
}

void ResourcePackageStore::add(const std::string &id, const std::shared_ptr<ResourcePackage> &resourcePackage)
{
    m_resourcePackages[id] = resourcePackage;
}


}
