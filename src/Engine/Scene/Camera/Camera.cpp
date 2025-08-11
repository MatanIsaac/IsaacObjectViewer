#include "Camera.h"
#include "Utility/Log.hpp"

namespace isaacObjectViewer
{
    Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
        : 
        m_Front(glm::vec3(0.0f, 0.0f, -1.0f)), 
        m_MovementSpeed(DEFAULT_CAMERA_SPEED), 
        m_Zoom(DEFAULT_CAMERA_ZOOM)
    {
        m_Position          = position;
        m_InitialPosition   = position;
        m_WorldUp           = up;
        m_Yaw               = yaw;
        m_Pitch             = pitch;
        UpdateVectors();
        m_Near              = 0.1f; // Default near plane
        m_Far               = 100.0f; // Default far plane
    }

    Camera::Camera(float posX, float posY, float posZ,float upX, 
                    float upY, float upZ, float yaw, float pitch)
        : 
        m_Front(glm::vec3(0.0f, 0.0f, -1.0f)), 
        m_MovementSpeed(DEFAULT_CAMERA_SPEED), 
        m_Zoom(DEFAULT_CAMERA_ZOOM)
    {
        m_Position          = glm::vec3(posX, posY, posZ);
        m_InitialPosition   = glm::vec3(posX, posY, posZ);
        m_WorldUp           = glm::vec3(upX, upY, upZ);
        m_Yaw               = yaw;
        m_Pitch             = pitch;
        UpdateVectors();
    }

    void Camera::ProcessKeyboard(CameraMovement direction, float deltaTime)
    {
        float velocity = m_MovementSpeed * deltaTime;
        if (direction == FORWARD)
            m_Position += m_Front * velocity;
        if (direction == BACKWARD)
            m_Position -= m_Front * velocity;
        if (direction == LEFT)
            m_Position -= m_Right * velocity;
        if (direction == RIGHT)
            m_Position += m_Right * velocity;
        if (direction == UP)
            m_Position += m_Up * velocity;
        if (direction == DOWN)
            m_Position -= m_Up * velocity;
    }

    void Camera::UpdateVectors()
    {
        // calculate the new Front vector
        glm::vec3 front;
        front.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
        front.y = sin(glm::radians(m_Pitch));
        front.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
        m_Front = glm::normalize(front);
        // also re-calculate the Right and Up vector
        m_Right = glm::normalize(glm::cross(
            m_Front, m_WorldUp));  // normalize the vectors, because their length gets closer to 0 the
                                // more you look up or down which results in slower movement.
        m_Up = glm::normalize(glm::cross(m_Right, m_Front));
    }

    void Camera::Update(float dt)
    {
        SDL_PumpEvents();
        
        const bool* keys = SDL_GetKeyboardState(nullptr);        
        if (keys[SDL_SCANCODE_W]) 
            ProcessKeyboard(FORWARD, dt);
        if (keys[SDL_SCANCODE_S]) 
            ProcessKeyboard(BACKWARD, dt);
        if (keys[SDL_SCANCODE_A]) 
            ProcessKeyboard(LEFT, dt);
        if (keys[SDL_SCANCODE_D]) 
            ProcessKeyboard(RIGHT, dt);
        if (keys[SDL_SCANCODE_E]) 
            ProcessKeyboard(UP, dt);
        if (keys[SDL_SCANCODE_Q]) 
            ProcessKeyboard(DOWN, dt);
    }
}