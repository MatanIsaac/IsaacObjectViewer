/**
 * @file Mouse.h
 * @brief Header file for the Mouse class.
 *  This class handles mouse input and provides functionality for mouse movement, scrolling, and clicking.
 */

#pragma once
#include "Utility/config.h"
#include "Camera/Camera.h"

namespace isaacObjectViewer
{
    class Mouse
    {
    public:
        /// @brief Default sensitivity value for mouse movement.
        static constexpr float DEFAULT_SENSITIVITY = 0.1f;

        /// @brief Gets the singleton instance of the Mouse class.
        /// @return The singleton instance of the Mouse class.
        static Mouse* GetInstance()
        {
            if(s_Instance == nullptr)
            {
                s_Instance = new Mouse();
            }
            return s_Instance;
        }   

        /// @brief Gets the first mouse state.
        /// @return The first mouse state.
        bool    &GetFirstMouse()                        { return m_FirstMouse; }

        /// @brief Gets the last mouse X position.
        /// @return The last mouse X position.
        float   &GetLastX()                             { return m_LastX; }
        
        /// @brief Gets the last mouse Y position.
        /// @return The last mouse Y position.
        float   &GetLastY()                             { return m_LastY; }

        /// @brief Gets the current mouse yaw angle.
        /// @return The current mouse yaw angle.
        float   &GetYaw()                               { return m_Yaw; }
        
        /// @brief Gets the current mouse pitch angle.
        /// @return The current mouse pitch angle.
        float   &GetPitch()                             { return m_Pitch; }
        
        /// @brief Gets the current mouse sensitivity.
        /// @return The current mouse sensitivity.
        float   &GetSensitivity()                       { return m_Sensitivity; }

        /// @brief Gets the current mouse ray tracing state.
        /// @return The current mouse ray tracing state.
        bool    &GetTraceMouseRay()                     { return m_TraceMouseRay; }

        /// @brief Sets the current mouse sensitivity.
        /// @param newSensitivity The new mouse sensitivity.
        void    SetSensitivity(float newSensitivity)    { m_Sensitivity = newSensitivity; }

        /// @brief Resets the current mouse sensitivity to the default value.
        void    ResetSensitivity()                      { m_Sensitivity = DEFAULT_SENSITIVITY; }

        /// @brief Processes mouse motion events.
        /// @param camera The camera to apply the motion to.
        /// @param xoffset The change in mouse X position.
        /// @param yoffset The change in mouse Y position.
        /// @param constrainPitch Whether to constrain the pitch angle.
        void ProcessMotion(Camera* camera,float xoffset, float yoffset, bool constrainPitch = true);
        
        /// @brief Processes mouse scroll events.
        /// @param yoffset The change in mouse Y position.
        /// @param camera The camera to apply the zoom to.
        void ProcessZoom(float yoffset, Camera* camera);

        /// @brief Processes mouse click events.
        /// @param mouseX The X position of the mouse.
        /// @param mouseY The Y position of the mouse.
        /// @param camera The camera to apply the click to.
        void ProcessMouseClick(float mouseX, float mouseY, Camera* camera);

    private:
        Mouse()
            : 
            m_FirstMouse(true),
            m_LastX(0.f),
            m_LastY(0.f),
            m_Pitch(0.0f),
            m_Sensitivity(DEFAULT_SENSITIVITY),
            m_Yaw(-90.0f),
            m_TraceMouseRay(false)
        {}
        ~Mouse() = default;

    private:
        static Mouse* s_Instance;
        bool m_FirstMouse;
        float m_LastX; 
        float m_LastY;
        float m_Pitch;
        float m_Sensitivity;
        float m_Yaw;
        bool m_TraceMouseRay;
        

    };
} // namespace isaacObjectViewer
