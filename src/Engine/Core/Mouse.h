#pragma once
#include "Engine/Scene/Camera/Camera.h"

namespace isaacGraphicsEngine
{
    class Mouse
    {
    public:
        static Mouse& GetInstance()
        {
            static Mouse instance;
            return instance;
        }   

        bool &GetFirstMouse() { return m_FirstMouse; }
        float &GetLastX() { return m_LastX; }
        float &GetLastY() { return m_LastY; }
        float &GetYaw() { return m_Yaw; }
        float &GetPitch() { return m_Pitch; }
        float &GetSensitivity() { return m_Sensitivity; }

        void ResetSensitivity() { m_Sensitivity = DEFAULT_SENSITIVITY; }

        // processes input received from a mouse input system. Expects the offset value in both the x
        // and y direction.
        void ProcessMovement(Camera* camera,float xoffset, float yoffset, GLboolean constrainPitch = true);

        // processes input received from a mouse scroll-wheel event. Only requires input on the vertical
        // wheel-axis
        void ProcessScroll(float yoffset, Camera* camera);
    private:
        Mouse()
            : 
            m_FirstMouse(true),
            m_Yaw(-90.0f),
            m_Pitch(0.0f),
            m_LastX(400.f),
            m_LastY(300.f),
            m_Sensitivity(DEFAULT_SENSITIVITY)
        {}
        ~Mouse() = default;

        // Delete the copy constructor and assignment operator to prevent copying of the singleton
        Mouse(const Mouse&) = delete;
        Mouse& operator=(const Mouse&) = delete;
    private:
        static Mouse* s_Instance;
        bool m_FirstMouse;
        float m_LastX; 
        float m_LastY;
        float m_Pitch;
        float m_Sensitivity;
        float m_Yaw; // yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction
                          // vector pointing to the right so we initially rotate a bit to the left.
        
        static constexpr float DEFAULT_SENSITIVITY = 0.1f;

    };
} // namespace isaacGraphicsEngine
