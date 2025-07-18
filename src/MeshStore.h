#pragma once

#include <glad/glad.h>

#include <vector>
#include <string>
#include <unordered_map>
#include <optional>
#include <memory>

namespace engine {

struct MeshData {
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

    auto get(const std::string& id) const -> std::optional<std::shared_ptr<MeshData>>;
    void add(const std::string& id, const std::shared_ptr<MeshData>& meshData);

private:
    std::unordered_map<std::string, std::shared_ptr<MeshData>> m_meshes;
};

}
