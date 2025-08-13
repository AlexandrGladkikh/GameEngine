#pragma once

#include <filesystem>
#include <memory>
#include <optional>

namespace engine {

struct MeshData;

auto buildMesh(const std::filesystem::path& path) -> std::optional<std::unique_ptr<MeshData>>;

}
