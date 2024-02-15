#include "Mesh.hpp"
#include "Logger.hpp"

#include <GL/glew.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>


std::optional<Texture> Texture::load(const std::string& file, const std::string& directory, Type type)
{
    uint32_t texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_set_flip_vertically_on_load(true);

    int32_t width, height, channels;
    uint8_t* data = stbi_load((directory + '/' + file).data(), &width, &height, &channels, 0);

    if (data == nullptr)
    {
        log("[Error] Failed to load texture: {}", file);
        return std::nullopt;
    }

    GLenum format = GL_RGB;

    if (channels == 1)
    {
        format = GL_RED;
    }
    else if (channels == 3)
    {
        format = GL_RGB;
    }
    else if (channels == 4)
    {
        format = GL_RGBA;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);

    return std::make_optional(Texture { texture, type, file });
}


Mesh Mesh::create(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, const std::vector<Texture>& textures)
{
    uint32_t vertexArray;
    glGenVertexArrays(1, &vertexArray);
    glBindVertexArray(vertexArray);

    uint32_t vertexBuffer;
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    uint32_t elementBuffer;
    glGenBuffers(1, &elementBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), indices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, position)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, normal)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, texCoords)));

    glBindVertexArray(0);

    Mesh self;

    self.m_vertices = vertices;
    self.m_indices = indices;
    self.m_textures = textures;
    self.m_vertexArray = vertexArray;
    self.m_vertexBuffer = vertexBuffer;
    self.m_elementBuffer = elementBuffer;

    return self;
}

Mesh::~Mesh()
{
    glDeleteVertexArrays(1, &m_vertexArray);
    glDeleteBuffers(1, &m_vertexBuffer);
    glDeleteBuffers(1, &m_elementBuffer);
}

Mesh::Mesh(Mesh&& other) noexcept
{
    std::swap(m_vertices, other.m_vertices);
    std::swap(m_indices, other.m_indices);
    std::swap(m_textures, other.m_textures);
    std::swap(m_vertexArray, other.m_vertexArray);
    std::swap(m_vertexBuffer, other.m_vertexBuffer);
    std::swap(m_elementBuffer, other.m_elementBuffer);
}

Mesh& Mesh::operator=(Mesh&& other) noexcept
{
    std::swap(m_vertices, other.m_vertices);
    std::swap(m_indices, other.m_indices);
    std::swap(m_textures, other.m_textures);
    std::swap(m_vertexArray, other.m_vertexArray);
    std::swap(m_vertexBuffer, other.m_vertexBuffer);
    std::swap(m_elementBuffer, other.m_elementBuffer);

    return *this;
}

void Mesh::draw(Shader& shader) const
{
    uint32_t diffuseNr = 1;
    uint32_t specularNr = 1;
    uint32_t normalNr = 1;
    uint32_t heightNr = 1;

    for (size_t idx = 0; idx < m_textures.size(); ++idx)
    {
        glActiveTexture(GL_TEXTURE0 + idx);

        std::string name;
        const Texture::Type type = m_textures[idx].type;

        if (type == Texture::Type::Diffuse)
        {
            name = std::string("texture_diffuse") + std::to_string(diffuseNr++);
        }
        else if (type == Texture::Type::Specular)
        {
            name = std::string("texture_specular") + std::to_string(specularNr++);
        }
        else if (type == Texture::Type::Normal)
        {
            name = std::string("texture_normal") + std::to_string(normalNr++);
        }
        else if (type == Texture::Type::Height)
        {
            name = std::string("texture_height") + std::to_string(heightNr++);
        }

        int32_t location = glGetUniformLocation(shader.getId(), name.c_str());

        if (location == -1)
        {
            log("[Warning] Uniform not found: {}", name);
            continue;
        }

        glUniform1i(location, idx);
        glBindTexture(GL_TEXTURE_2D, m_textures[idx].id);
    }

    glBindVertexArray(m_vertexArray);
    glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);

    glActiveTexture(GL_TEXTURE0);
}
