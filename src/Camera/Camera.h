#pragma once

#include "Utility/config.h"


namespace isaacObjectLoader
{
enum CameraMovement
{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

class Camera
{
public:
    // Default camera values
    static constexpr float DEFAULT_YAW         = -90.0f;
    static constexpr float DEFAULT_PITCH       = 0.0f;
    static constexpr float DEFAULT_SPEED       = 2.5f;
    static constexpr float DEFAULT_SENSITIVITY = 0.1f;
    static constexpr float DEFAULT_ZOOM        = 45.0f;

    // constructor with vectors
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
           glm::vec3 up       = glm::vec3(0.0f, 1.0f, 0.0f),
           float     yaw      = DEFAULT_YAW,
           float     pitch    = DEFAULT_PITCH);

    // constructor with scalar values
    Camera(float posX,
           float posY,
           float posZ,
           float upX,
           float upY,
           float upZ,
           float yaw,
           float pitch);

    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix() { return glm::lookAt(m_Position, m_Position + m_Front, m_Up); }

    // processes input received from any keyboard-like input system. Accepts input parameter in the
    // form of camera defined ENUM (to abstract it from windowing systems)
    void ProcessKeyboard(CameraMovement direction, float deltaTime);

    // processes input received from a mouse input system. Expects the offset value in both the x
    // and y direction.
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);

    // processes input received from a mouse scroll-wheel event. Only requires input on the vertical
    // wheel-axis
    void ProcessMouseScroll(float yoffset);

    float&     GetMouseSensitivity() { return m_MouseSensitivity; }
    float&     GetCameraZoom() { return m_Zoom; }
    float&     GetCameraSpeed() { return m_MovementSpeed; }
    glm::vec3& GetCameraPosition() { return m_Position; }

    void ResetCameraSpeed() { m_MovementSpeed = DEFAULT_SPEED; }
    void ResetMouseSensitivity() { m_MouseSensitivity = DEFAULT_SENSITIVITY; }
    void SetCameraSpeed(float newSpeed) { m_MovementSpeed = newSpeed; }
    void SetCameraPosition(const glm::vec3& newPosition) { m_Position = newPosition; }

private:
    // calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors();

private:
    // camera Attributes
    glm::vec3 m_Position;
    glm::vec3 m_Front;
    glm::vec3 m_Up;
    glm::vec3 m_Right;
    glm::vec3 m_WorldUp;
    // euler Angles
    float m_Yaw;
    float m_Pitch;
    // camera options
    float m_MovementSpeed;
    float m_MouseSensitivity;
    float m_Zoom;
};

}