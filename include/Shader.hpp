#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <string_view>

#include <optional>


class Shader
{
public:
    static std::optional<Shader> create(std::string_view vertexPath, std::string_view fragmentPath);

    ~Shader();

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    Shader(Shader&& other) noexcept;
    Shader& operator=(Shader&& other) noexcept;

    uint32_t getId() const;

    void use() const;

    void setBool(std::string_view name, bool value) const;
    void setInt(std::string_view name, int value) const;
    void setFloat(std::string_view name, float value) const;
    void setMat4(std::string_view name, const glm::mat4& value) const;
    void setVec3(std::string_view name, const glm::vec3& value) const;

private:
    Shader(uint32_t id);

    uint32_t m_id = 0;
};