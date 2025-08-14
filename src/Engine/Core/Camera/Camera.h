/** @file Camera.h
 *  @brief Contains the definition of the Camera class.
 *  This class is responsible for handling the camera's position, orientation,
 *  and projection in the 3D scene.
 */

#pragma once

#include "Utility/config.h"

namespace isaacObjectViewer
{
    /// @brief Enumeration for camera movement directions.
    enum CameraMovement
    {
        FORWARD,
        BACKWARD,
        LEFT,
        RIGHT,
        UP,
        DOWN
    };

    class Camera
    {
    public:
        // Default camera values
        static constexpr float DEFAULT_CAMERA_YAW         = -90.0f;
        static constexpr float DEFAULT_CAMERA_PITCH       = 0.0f;
        static constexpr float DEFAULT_CAMERA_SPEED       = 5.0f;
        static constexpr float DEFAULT_CAMERA_ZOOM        = 45.0f;

        /// @brief Constructor with vector parameters.
        /// @param position The initial position of the camera.
        /// @param up The up vector of the camera.
        /// @param yaw The initial yaw angle of the camera.
        /// @param pitch The initial pitch angle of the camera.
        Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3 up       = glm::vec3(0.0f, 1.0f, 0.0f),
            float     yaw      = DEFAULT_CAMERA_YAW,
            float     pitch    = DEFAULT_CAMERA_PITCH);

        /// @brief Constructor with scalar parameters.
        /// @param posX The initial x position of the camera.
        /// @param posY The initial y position of the camera.
        /// @param posZ The initial z position of the camera.
        /// @param upX The up vector x component.
        /// @param upY The up vector y component.
        /// @param upZ The up vector z component.
        /// @param yaw The initial yaw angle of the camera.
        /// @param pitch The initial pitch angle of the camera.
        Camera(float posX,
            float posY,
            float posZ,
            float upX,
            float upY,
            float upZ,
            float yaw,
            float pitch);

        /// @brief Gets the view matrix of the camera.
        /// The view matrix is calculated using Euler Angles and the LookAt Matrix
        /// @return The view matrix.
        glm::mat4 GetViewMatrix() { return glm::lookAt(m_Position, m_Position + m_Front, m_Up); }

        /// @brief Sets the projection matrix of the camera.
        /// @param aspect The aspect ratio of the viewport.
        void SetProjection(float aspect)
        {
            m_Projection = glm::perspective(glm::radians(m_Zoom), aspect, m_Near, m_Far);
        }

        /// @brief Gets the projection matrix of the camera.
        /// @return The projection matrix.
        const glm::mat4& GetProjectionMatrix() const { return m_Projection; }

        /// @brief Processes keyboard input for camera movement.
        /// @param direction The direction of movement.
        /// @param deltaTime The time elapsed since the last frame.
        void ProcessKeyboard(CameraMovement direction, float deltaTime);

        /// @brief Gets the pitch angle of the camera.
        /// @return The pitch angle.
        const float&     GetPitch() { return m_Pitch; }

        /// @brief Gets the yaw angle of the camera.
        /// @return The yaw angle.
        const float&     GetYaw() { return m_Yaw; }

        /// @brief Gets the zoom level of the camera.
        /// @return The zoom level.
        float&     GetZoom() { return m_Zoom; } 

        /// @brief Gets the movement speed of the camera.
        /// @return The movement speed.
        float&     GetSpeed() { return m_MovementSpeed; }

        /// @brief Gets the position of the camera.
        /// @return The position of the camera.
        const glm::vec3& GetPosition() { return m_Position; }

        /// @brief Resets the speed of the camera to the default value.
        void ResetSpeed() { m_MovementSpeed = DEFAULT_CAMERA_SPEED; }

        /// @brief Resets the position of the camera to the initial position.
        void ResetPosition() { m_Position = m_InitialPosition; }

        /// @brief Sets the speed of the camera.
        /// @param newSpeed The new speed of the camera.
        void SetSpeed(float newSpeed) { m_MovementSpeed = newSpeed; }

        /// @brief Sets the position of the camera.
        /// @param newPosition The new position of the camera.
        void SetPosition(const glm::vec3& newPosition) { m_Position = newPosition; }

        /// @brief Sets the pitch angle of the camera.
        /// @param newPitch The new pitch angle of the camera.
        void SetPitch(float newPitch) { m_Pitch = newPitch; }
        
        /// @brief Sets the yaw angle of the camera.
        /// @param newYaw The new yaw angle of the camera.
        void SetYaw(float newYaw) { m_Yaw = newYaw; }

        /// @brief Sets the zoom level of the camera.
        /// @param newZoom The new zoom level of the camera.
        void SetZoom(float newZoom) { m_Zoom = newZoom; }

        /// @brief Gets the near clipping plane distance of the camera.
        /// @return The near clipping plane distance.
        float GetNear() const { return m_Near; }

        /// @brief Gets the far clipping plane distance of the camera.
        /// @return The far clipping plane distance.
        float GetFar() const { return m_Far; }

        /// @brief Adds to the pitch angle of the camera.
        /// @param amount The amount to add to the pitch angle.
        void AddPitch(float amount) { m_Pitch += amount; }

        /// @brief Adds to the yaw angle of the camera.
        /// @param amount The amount to add to the yaw angle.
        void AddYaw(float amount) { m_Yaw += amount; }

        /// @brief Adds to the zoom level of the camera.
        /// @param amount The amount to add to the zoom level.
        void AddZoom(float amount) { m_Zoom += amount; }

        /// @brief Adds to the movement speed of the camera.
        /// @param amount The amount to add to the movement speed.
        void AddSpeed(float amount) { m_MovementSpeed += amount; }
        
        /// @brief Updates the camera's front, right, and up vectors based on the current yaw and pitch.
        void UpdateVectors();

        /// @brief Updates the camera's position based on the current speed and direction.
        /// @param dt The delta time since the last update.
        void Update(float dt);

        /// @brief Looks at a target point in world space.
        /// @param target The target point to look at.
        void LookAtTarget(const glm::vec3& target);

    private:
        // camera Attributes
        glm::mat4 m_Projection;
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
        float m_Near;
        float m_Far;
    };

}