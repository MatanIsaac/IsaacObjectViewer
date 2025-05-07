#pragma once

#include "Utility/config.h"

namespace isaacGraphicsEngine
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
        static constexpr float DEFAULT_SPEED       = 5.0f;
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

        const float&     GetPitch() { return m_Pitch; }
        const float&     GetYaw() { return m_Yaw; }
        float&     GetZoom() { return m_Zoom; }
        float&     GetSpeed() { return m_MovementSpeed; }
        const glm::vec3& GetPosition() { return m_Position; }

        void ResetSpeed() { m_MovementSpeed = DEFAULT_SPEED; }
        void ResetPosition() { m_Position = m_InitialPosition; }
        void SetSpeed(float newSpeed) { m_MovementSpeed = newSpeed; }
        void SetPosition(const glm::vec3& newPosition) { m_Position = newPosition; }
        void SetPitch(float newPitch) { m_Pitch = newPitch; }
        void SetYaw(float newYaw) { m_Yaw = newYaw; }
        void SetZoom(float newZoom) { m_Zoom = newZoom; }

        void AddPitch(float amount) { m_Pitch += amount; }
        void AddYaw(float amount) { m_Yaw += amount; }
        void AddZoom(float amount) { m_Zoom += amount; }
        void AddSpeed(float amount) { m_MovementSpeed += amount; }
        
        /**
         * @brief calculates the front vector from the Camera's (updated) Euler Angles
         */
        void UpdateVectors();

        void Update(float dt);
    private:
        // camera Attributes
        glm::vec3 m_InitialPosition;
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
        float m_Zoom;


    };

}