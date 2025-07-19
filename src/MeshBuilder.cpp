#include "MeshBuilder.h"
#include <MeshStore.h>
#include "FileSystem.h"

#include <glad/glad.h>

#include <cstring>

namespace engine {

auto buildMeshGL(const std::vector<GLfloat>& vertices,
        const std::vector<GLuint>& indices,
        GLuint stride,
        GLuint offset) -> std::unique_ptr<MeshData>
{
    auto data = std::make_unique<MeshData>();
    glGenVertexArrays(1, &data->VAO);
    glGenBuffers(1, &data->VBO);
    glGenBuffers(1, &data->EBO);

    glBindVertexArray(data->VAO);

    glBindBuffer(GL_ARRAY_BUFFER, data->VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size(), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride * sizeof(GLfloat), (void*)offset);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size(), indices.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    return data;
}

auto buildMesh(const std::filesystem::path& path) -> std::optional<std::unique_ptr<MeshData>>
{
    auto pathVertices = path / "vertices.bin";
    auto pathIndices = path / "indices.bin";

    auto vertices_file = FileSystem::file(pathVertices, std::ios::out | std::ios::binary);
    auto vertices_data = vertices_file.readBinary();
    if (vertices_data.empty()) {
        return std::nullopt;
    }

    auto indices_file = FileSystem::file(pathIndices, std::ios::out | std::ios::binary);
    auto indices_data = indices_file.readBinary();
    if (indices_data.empty()) {
        return std::nullopt;
    }

    std::vector<GLfloat> vertices_data_gl(vertices_data.size() / sizeof(GLfloat), 0);
    std::memcpy(vertices_data_gl.data(), vertices_data.data(), vertices_data.size());

    std::vector<GLuint> indices_data_gl(indices_data.size() / sizeof(GLuint), 0);
    std::memcpy(indices_data_gl.data(), indices_data.data(), indices_data.size());

    return buildMeshGL(vertices_data_gl, indices_data_gl, 5, 0);
}

}
