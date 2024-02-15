#include "Model.hpp"

#include "Logger.hpp"

#include <algorithm>



std::optional<Model> Model::create(const std::string_view& path)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path.data(), aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        log("Assimp error: ", importer.GetErrorString());
        return std::nullopt;
    }

    Model model;

    model.m_directory = path.substr(0, path.find_last_of('/'));
    model.processNode(scene->mRootNode, scene);

    return std::make_optional(std::move(model));
}

void Model::draw(Shader& shader) const
{
    for (const Mesh& mesh : m_meshes)
    {
        mesh.draw(shader);
    }
}

void Model::processNode(const aiNode* node, const aiScene* scene)
{
    for (size_t idx = 0; idx < node->mNumMeshes; ++idx)
    {
        const aiMesh* mesh = scene->mMeshes[node->mMeshes[idx]];
        m_meshes.emplace_back(processMesh(mesh, scene));
    }

    for (size_t idx = 0; idx < node->mNumChildren; ++idx)
    {
        processNode(node->mChildren[idx], scene);
    }
}

Mesh Model::processMesh(const aiMesh* mesh, const aiScene* scene)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    for (size_t idx = 0; idx < mesh->mNumVertices; ++idx)
    {
        Vertex vertex;

        vertex.position = glm::vec3(mesh->mVertices[idx].x, mesh->mVertices[idx].y, mesh->mVertices[idx].z);

        if (mesh->mNormals)
        {
            vertex.normal = glm::vec3(mesh->mNormals[idx].x, mesh->mNormals[idx].y, mesh->mNormals[idx].z);
        }
        else
        {
            vertex.normal = glm::vec3(0.0f, 0.0f, 0.0f);
        }

        if (mesh->mTextureCoords[0])
        {
            vertex.texCoords = glm::vec2(mesh->mTextureCoords[0][idx].x, mesh->mTextureCoords[0][idx].y);
        }
        else
        {
            vertex.texCoords = glm::vec2(0.0f, 0.0f);
        }

        vertices.push_back(vertex);
    }

    for (size_t idx = 0; idx < mesh->mNumFaces; ++idx)
    {
        const aiFace& face = mesh->mFaces[idx];

        for (size_t j = 0; j < face.mNumIndices; ++j)
        {
            indices.push_back(face.mIndices[j]);
        }
    }

    const aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

    std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, Texture::Type::Diffuse);
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

    std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, Texture::Type::Specular);
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

    std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, Texture::Type::Normal);
    textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

    std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, Texture::Type::Height);
    textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

    return Mesh::create(vertices, indices, textures);
}

std::vector<Texture> Model::loadMaterialTextures(const aiMaterial* mat, const aiTextureType aiType, Texture::Type type)
{
    std::vector<Texture> textures;

    for (size_t idx = 0; idx < mat->GetTextureCount(aiType); ++idx)
    {
        aiString str;
        mat->GetTexture(aiType, idx, &str);

        auto predicate = [&str](const Texture& texture) { return std::strcmp(texture.file.data(), str.C_Str()) == 0; };
        auto it = std::find_if(m_loadedTextures.begin(), m_loadedTextures.end(), predicate);

        if (it != m_loadedTextures.end())
        {
            textures.push_back(*it);
            continue;
        }

        std::string file = str.C_Str();
        file = file.substr(file.find_last_of('/') + 1, file.size() - 1);

        auto textureOpt = Texture::load(file, std::string(m_directory), type);

        if (!textureOpt)
        {
            log("[Error] Failed to load texture: ", str.C_Str());
            continue;
        }

        m_loadedTextures.push_back(*textureOpt);
        textures.push_back(*textureOpt);
    }

    return textures;
}
