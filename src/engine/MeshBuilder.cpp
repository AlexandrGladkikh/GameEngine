#include "MeshBuilder.h"
#include "MeshStore.h"
#include "FileSystem.h"

#include <glad/glad.h>
#include <rapidjson/document.h>

#include <cstring>
#include <cstdint>

namespace engine {

struct MeshConfig {
    GLuint stride;
    std::optional<GLuint> vertices_offset;
    std::optional<GLuint> vertices_size;
    std::optional<GLuint> texture_coords_offset;
    std::optional<GLuint> texture_coords_size;
    std::optional<GLuint> normals_offset;
    std::optional<GLuint> normals_size;
};

auto buildMeshGL(const std::string& name,
    const std::vector<GLfloat>& vertices,
    const std::vector<GLuint>& indices,
    const MeshConfig& mesh_config) -> std::unique_ptr<MeshData>
{
    auto data = std::make_unique<MeshData>();

    data->name = name;

    glGenVertexArrays(1, &data->VAO);
    glGenBuffers(1, &data->VBO);
    glGenBuffers(1, &data->EBO);

    glBindVertexArray(data->VAO);

    glBindBuffer(GL_ARRAY_BUFFER, data->VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);

    if (mesh_config.vertices_size.has_value() && mesh_config.vertices_offset.has_value()) {
        glVertexAttribPointer(0, mesh_config.vertices_size.value(), GL_FLOAT, GL_FALSE, mesh_config.stride * sizeof(GLfloat), (void*)(uintptr_t)mesh_config.vertices_offset.value());
        glEnableVertexAttribArray(0);
    }

    if (mesh_config.texture_coords_size.has_value() && mesh_config.texture_coords_offset.has_value()) {
        glVertexAttribPointer(1, mesh_config.texture_coords_size.value(), GL_FLOAT, GL_FALSE, mesh_config.stride * sizeof(GLfloat), (void*)(uintptr_t)mesh_config.texture_coords_offset.value());
        glEnableVertexAttribArray(1);
    }

    if (mesh_config.normals_size.has_value() && mesh_config.normals_offset.has_value()) {
        glVertexAttribPointer(2, mesh_config.normals_size.value(), GL_FLOAT, GL_FALSE, mesh_config.stride * sizeof(GLfloat), (void*)(uintptr_t)mesh_config.normals_offset.value());
        glEnableVertexAttribArray(2);
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    return data;
}

auto buildMesh(const std::filesystem::path& path) -> std::optional<std::unique_ptr<MeshData>>
{
    auto pathVertices = path / "vertices.bin";
    auto pathIndices = path / "indices.bin";
    auto pathConfig = path / "config.json";

    if (!FileSystem::exists(pathConfig) || !FileSystem::isFile(pathConfig) ||
        !FileSystem::exists(pathVertices) || !FileSystem::isFile(pathVertices) ||
        !FileSystem::exists(pathIndices) || !FileSystem::isFile(pathIndices)) {
        return std::nullopt;
    }

    auto config_file = FileSystem::file(pathConfig, std::ios::in);
    auto config_data = config_file.readText();
    if (config_data.empty()) {
        return std::nullopt;
    }

    rapidjson::Document config_document;
    config_document.Parse(config_data.c_str());
    if (config_document.HasParseError()) {
        return std::nullopt;
    }

    auto stride = config_document["stride"].GetUint();
    auto vertices_config = config_document["vertices"].GetObject();
    auto texture_coords_config = config_document["textureCoords"].GetObject();
    auto normals_config = config_document["normals"].GetObject();

    std::optional<GLuint> vertices_offset;
    std::optional<GLuint> vertices_size;
    std::optional<GLuint> texture_coords_offset;
    std::optional<GLuint> texture_coords_size;
    std::optional<GLuint> normals_offset;
    std::optional<GLuint> normals_size;

    if (vertices_config.HasMember("offset") && vertices_config.HasMember("size")) {
        vertices_offset = vertices_config["offset"].GetUint();
        vertices_size = vertices_config["size"].GetUint();
    }
    if (texture_coords_config.HasMember("offset") && texture_coords_config.HasMember("size")) {
        texture_coords_offset = texture_coords_config["offset"].GetUint();
        texture_coords_size = texture_coords_config["size"].GetUint();
    }
    if (normals_config.HasMember("offset") && normals_config.HasMember("size")) {
        normals_offset = normals_config["offset"].GetUint();
        normals_size = normals_config["size"].GetUint();
    }
    MeshConfig mesh_config = { stride, vertices_offset, vertices_size, texture_coords_offset, texture_coords_size, normals_offset, normals_size };

    auto vertices_file = FileSystem::file(pathVertices, std::ios::in | std::ios::binary);
    auto vertices_data = vertices_file.readBinary();
    if (vertices_data.empty()) {
        return std::nullopt;
    }

    auto indices_file = FileSystem::file(pathIndices, std::ios::in | std::ios::binary);
    auto indices_data = indices_file.readBinary();
    if (indices_data.empty()) {
        return std::nullopt;
    }

    std::vector<GLfloat> vertices_data_gl(vertices_data.size() / sizeof(GLfloat), 0);
    std::memcpy(vertices_data_gl.data(), vertices_data.data(), vertices_data.size());

    std::vector<GLuint> indices_data_gl(indices_data.size() / sizeof(GLuint), 0);
    std::memcpy(indices_data_gl.data(), indices_data.data(), indices_data.size());

    return buildMeshGL(path.stem().string(), vertices_data_gl, indices_data_gl, mesh_config);
}

}
