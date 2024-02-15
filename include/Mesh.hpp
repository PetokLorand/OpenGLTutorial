#pragma once

#include "Shader.hpp"

#include <glm/glm.hpp>

#include <string>
#include <vector>
#include <optional>


struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
};

struct Texture
{
    enum class Type
    {
        Diffuse,
        Specular,
        Normal,
        Height
    };

    uint32_t id;
    Type type;
    std::string file;

    static std::optional<Texture> load(const std::string& file, const std::string& directory, Type type);
};

class Mesh
{
public:
    static Mesh create(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, const std::vector<Texture>& textures);
    ~Mesh();

    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;

    Mesh(Mesh&& other) noexcept;
    Mesh& operator=(Mesh&& other) noexcept;

    void draw(Shader& shader) const;

private:
    Mesh() = default;

    std::vector<Vertex> m_vertices;
    std::vector<uint32_t> m_indices;
    std::vector<Texture> m_textures;

    uint32_t m_vertexArray = 0;
    uint32_t m_vertexBuffer = 0;
    uint32_t m_elementBuffer = 0;
};