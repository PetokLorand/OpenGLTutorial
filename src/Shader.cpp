#include "Shader.hpp"

#include "Logger.hpp"

#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <cstdio>
#include <cstdlib>


std::optional<Shader> Shader::create(std::string_view vertexPath, std::string_view fragmentPath)
{
    FILE* vertexFile = std::fopen(vertexPath.data(), "rb");

    if (vertexFile == nullptr)
    {
        log("[Error] Failed to open vertex shader file: {}", vertexPath);
        return std::nullopt;
    }

    std::fseek(vertexFile, 0, SEEK_END);
    const size_t vertexSourceSize = std::ftell(vertexFile);
    std::fseek(vertexFile, 0, SEEK_SET);

    std::string vertexShadersource(vertexSourceSize, '\0');
    size_t size = std::fread(vertexShadersource.data(), vertexSourceSize, 1, vertexFile);

    if (size != 1)
    {
        log("[Error] Failed to read vertex shader file: {}", vertexPath);
        return std::nullopt;
    }

    std::fclose(vertexFile);

    FILE* fragmentFile = std::fopen(fragmentPath.data(), "rb");

    if (fragmentFile == nullptr)
    {
        log("[Error] Failed to open fragment shader file: {}", fragmentPath);
        return std::nullopt;
    }

    std::fseek(fragmentFile, 0, SEEK_END);
    const size_t fragmentSourceSize = std::ftell(fragmentFile);
    std::fseek(fragmentFile, 0, SEEK_SET);

    std::string fragmentShadersource(fragmentSourceSize, '\0');
    size = std::fread(fragmentShadersource.data(), fragmentSourceSize, 1, fragmentFile);

    if (size != 1)
    {
        log("[Error] Failed to read fragment shader file: {}", fragmentPath);
        return std::nullopt;
    }

    std::fclose(fragmentFile);


    uint32_t vertexShader;
    uint32_t fragmentShader;
    int32_t result;
    char infoLog[512];

    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    const char* vertexSource = vertexShadersource.c_str();
    glShaderSource(vertexShader, 1, &vertexSource, nullptr);
    glCompileShader(vertexShader);
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);

    if (!result)
    {
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        log("[Error] Vertex shader compilation failed:\n {}", infoLog);
        return std::nullopt;
    }

    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    const char* fragmentSource = fragmentShadersource.c_str();
    glShaderSource(fragmentShader, 1, &fragmentSource, nullptr);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);

    if (!result)
    {
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        log("[Error] Fragment shader compilation failed:\n {}", infoLog);
        return std::nullopt;
    }

    uint32_t id = glCreateProgram();

    glAttachShader(id, vertexShader);
    glAttachShader(id, fragmentShader);
    glLinkProgram(id);
    glGetProgramiv(id, GL_LINK_STATUS, &result);

    if (!result)
    {
        glGetProgramInfoLog(id, 512, nullptr, infoLog);
        log("[Error] Shader program linking failed:\n {}", infoLog);
        return std::nullopt;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return Shader { id };
}

Shader::Shader(uint32_t id) : m_id(id) {}

Shader::~Shader()
{
    glDeleteProgram(m_id);
}

Shader::Shader(Shader&& other) noexcept
{
    std::swap(m_id, other.m_id);
}

Shader& Shader::operator=(Shader&& other) noexcept
{
    std::swap(m_id, other.m_id);
    return *this;
}

uint32_t Shader::getId() const
{
    return m_id;
}

void Shader::use() const
{
    glUseProgram(m_id);
}

void Shader::setBool(std::string_view name, bool value) const
{
    glUniform1i(glGetUniformLocation(m_id, name.data()), static_cast<int>(value));
}

void Shader::setInt(std::string_view name, int value) const
{
    glUniform1i(glGetUniformLocation(m_id, name.data()), value);
}

void Shader::setFloat(std::string_view name, float value) const
{
    glUniform1f(glGetUniformLocation(m_id, name.data()), value);
}

void Shader::setMat4(std::string_view name, const glm::mat4& value) const
{
    int32_t location = glGetUniformLocation(m_id, name.data());

    if (location == -1)
    {
        log("[Warning] Uniform not found: {}", name);
        return;
    }

    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::setVec3(std::string_view name, const glm::vec3& value) const
{
    int32_t location = glGetUniformLocation(m_id, name.data());;

    if (location == -1)
    {
        log("[Warning] Uniform not found: {}", name);
        return;
    }

    glUniform3fv(location, 1, glm::value_ptr(value));
}