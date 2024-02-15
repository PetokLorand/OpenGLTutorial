#pragma once

#include "Mesh.hpp"
#include "Shader.hpp"

#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include <vector>
#include <optional>
#include <string_view>


class Model
{
public:
    static std::optional<Model> create(const std::string_view& path);

    Model(const Model&) = delete;
    Model& operator=(const Model&) = delete;

    Model(Model&&) noexcept = default;
    Model& operator=(Model&&) noexcept = default;

    void draw(Shader& shader) const;

private:
    Model() = default;

    void processNode(const aiNode* node, const aiScene* scene);
    Mesh processMesh(const aiMesh* mesh, const aiScene* scene);
    std::vector<Texture> loadMaterialTextures(const aiMaterial* mat, const aiTextureType aiType, Texture::Type type);

    std::vector<Mesh> m_meshes;
    std::string_view m_directory;
    std::vector<Texture> m_loadedTextures;
};