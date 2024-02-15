#version 410 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

struct Fragment
{
    vec3 position;
    vec3 normal;
    vec2 texCoords;
};

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out Fragment fragment;

void main()
{
    fragment.position = vec3(model * vec4(aPos, 1.0));
    gl_Position = projection * view * vec4(fragment.position, 1.0);
    fragment.normal = mat3(transpose(inverse(model))) * aNormal;
    fragment.texCoords = aTexCoords;
}
