#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


constexpr float YAW = -90.0f;
constexpr float PITCH = 0.0f;
constexpr float MOVEMENT_SPEED = 3.0f;
constexpr float LOOKING_SPEED = 1.0f;
constexpr float SENSITIVITY = 0.1f;
constexpr float ZOOM = 45.0f;


class Camera
{
public:
    enum class Movement
    {
        Forward,
        Backward,
        Left,
        Right,
        Up,
        Down
    };

    enum class Looking
    {
        Left,
        Right,
        Up,
        Down
    };

    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH);
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch);

    glm::mat4 getViewMatrix() const;

    float getZoom() const;
    const glm::vec3& getPosition() const;

    void processKeyboard(Movement direction, float deltaTime);
    void processKeyboard(Looking direction, float deltaTime);

    void processMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);
    void processMouseScroll(float yoffset);

private:

    void updateCameraVectors();

    glm::vec3 m_position;
    glm::vec3 m_front;
    glm::vec3 m_up;
    glm::vec3 m_right;
    glm::vec3 m_worldUp;

    float m_yaw;
    float m_pitch;

    float m_movementSpeed;
    float m_mouseSensitivity;
    float m_zoom;
};