#pragma once
#include "Utility/config.h"
#include "Scene/Camera/Camera.h"

namespace isaacObjectViewer
{
    class Mouse
    {
    public:
        static constexpr float DEFAULT_SENSITIVITY = 0.1f;

        static Mouse* GetInstance()
        {
            if(s_Instance == nullptr)
            {
                s_Instance = new Mouse();
            }
            return s_Instance;
        }   

        bool    &GetFirstMouse()                        { return m_FirstMouse; }
        float   &GetLastX()                             { return m_LastX; }
        float   &GetLastY()                             { return m_LastY; }
        float   &GetYaw()                               { return m_Yaw; }
        float   &GetPitch()                             { return m_Pitch; }
        float   &GetSensitivity()                       { return m_Sensitivity; }
        bool    &GetTraceMouseRay()                     { return m_TraceMouseRay; }

        void    SetSensitivity(float newSensitivity)    { m_Sensitivity = newSensitivity; }
        void    ResetSensitivity()                      { m_Sensitivity = DEFAULT_SENSITIVITY; }

        void ProcessMotion(Camera* camera,float xoffset, float yoffset, bool constrainPitch = true);
        void ProcessZoom(float yoffset, Camera* camera);
        void ProcessMouseClick(float mouseX, float mouseY, Camera* camera);
    private:
        Mouse()
            : 
            m_FirstMouse(true),
            m_LastX(400.f),
            m_LastY(300.f),
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
} // namespace isaacGraphicsEngine
