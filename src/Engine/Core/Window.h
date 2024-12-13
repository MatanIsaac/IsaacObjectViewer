#pragma once
#include "Utility/config.h"
#include "Engine/Scene/Camera/Camera.h"

namespace isaacGraphicsEngine
{
    class Window 
    {
    public:
        Window(const char *title, int width, int height, bool fullscreen = false);
        ~Window();

        GLFWwindow* GetGLFWwindow() {return m_Window; };
    private:
        GLFWwindow *m_Window;
        GLFWmonitor *m_PrimaryMonitor = nullptr;
    };

    


}