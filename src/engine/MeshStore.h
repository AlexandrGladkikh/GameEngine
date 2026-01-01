#pragma once

#include <glad/glad.h>

#include <vector>
#include <unordered_map>
#include <optional>
#include <memory>
#include <string>

namespace engine {

struct MeshData {
    std::string name;

    std::vector<GLfloat> vertices;
    std::vector<GLuint> indices;

    GLuint stride = 0;
    GLuint offset = 0;

    GLuint VAO = 0;
    GLuint VBO = 0;
    GLuint EBO = 0;

    void bind() const;
    void unbind() const;
};

class MeshStore final {
public:
    MeshStore() = default;
    ~MeshStore() = default;
    MeshStore(const MeshStore&) = delete;
    MeshStore(MeshStore&&) = delete;
    MeshStore& operator=(const MeshStore&) = delete;
    MeshStore& operator=(MeshStore&&) = delete;

    auto get(uint32_t id) const -> std::optional<std::shared_ptr<MeshData>>;
    auto get(const std::string& name) const -> std::optional<std::shared_ptr<MeshData>>;
    auto getIdByName(const std::string& name) const -> std::optional<uint32_t>;
    void add(uint32_t id, const std::shared_ptr<MeshData>& meshData);
    void remove(uint32_t id);

private:
    std::unordered_map<uint32_t, std::shared_ptr<MeshData>> m_meshes;
};

}
