#include "Camera.hpp"


Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
    : m_front(glm::vec3(0.0f, 0.0f, -1.0f))
    , m_movementSpeed(MOVEMENT_SPEED)
    , m_mouseSensitivity(SENSITIVITY)
    , m_zoom(ZOOM)
{
    m_position = position;
    m_worldUp = up;
    m_yaw = yaw;
    m_pitch = pitch;
    updateCameraVectors();
}

Camera::Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch)
    : m_front(glm::vec3(0.0f, 0.0f, -1.0f))
    , m_movementSpeed(MOVEMENT_SPEED)
    , m_mouseSensitivity(SENSITIVITY)
    , m_zoom(ZOOM)
{
    m_position = glm::vec3(posX, posY, posZ);
    m_worldUp = glm::vec3(upX, upY, upZ);
    m_yaw = yaw;
    m_pitch = pitch;
    updateCameraVectors();
}

glm::mat4 Camera::getViewMatrix() const
{
    return glm::lookAt(m_position, m_position + m_front, m_up);
}

float Camera::getZoom() const
{
    return m_zoom;
}

const glm::vec3& Camera::getPosition() const
{
    return m_position;
}

void Camera::processKeyboard(Movement direction, float deltaTime)
{
    const float velocity = m_movementSpeed * deltaTime;
    switch (direction)
    {
        break; case Movement::Forward: m_position += m_front * velocity;
        break; case Movement::Backward: m_position -= m_front * velocity;
        break; case Movement::Left: m_position -= m_right * velocity;
        break; case Movement::Right: m_position += m_right * velocity;
        break; case Movement::Up: m_position += m_up * velocity;
        break; case Movement::Down: m_position -= m_up * velocity;
    }
}

void Camera::processKeyboard(Looking direction, float deltaTime)
{
    static_cast<void>(deltaTime);

    switch (direction)
    {
        break; case Looking::Left: m_yaw -= LOOKING_SPEED;
        break; case Looking::Right: m_yaw += LOOKING_SPEED;
        break; case Looking::Up: m_pitch -= LOOKING_SPEED;
        break; case Looking::Down: m_pitch += LOOKING_SPEED;
    }

    updateCameraVectors();
}

void Camera::processMouseMovement(float xoffset, float yoffset, bool constrainPitch)
{
    xoffset *= m_mouseSensitivity;
    yoffset *= m_mouseSensitivity;

    m_yaw += xoffset;
    m_pitch += yoffset;

    if (constrainPitch)
    {
        if (m_pitch > 89.0f)
        {
            m_pitch = 89.0f;
        }
        else if (m_pitch < -89.0f)
        {
            m_pitch = -89.0f;
        }
    }

    updateCameraVectors();
}

void Camera::processMouseScroll(float yoffset)
{
    m_zoom -= yoffset;

    if (m_zoom <= 1.0f)
    {
        m_zoom = 1.0f;
    }
    else if (m_zoom >= 45.0f)
    {
        m_zoom = 45.0f;
    }
}

void Camera::updateCameraVectors()
{
    glm::vec3 front;
    front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    front.y = sin(glm::radians(m_pitch));
    front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    m_front = glm::normalize(front);

    m_right = glm::normalize(glm::cross(m_front, m_worldUp));
    m_up = glm::normalize(glm::cross(m_right, m_front));
}
