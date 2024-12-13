#include "Window.h"

namespace isaacGraphicsEngine 
{

    Window::Window(const char *title, int width, int height, bool fullscreen)
    {
        if (!glfwInit()) 
        {
            throw std::runtime_error("Failed to initialize GLFW");
        }
        if(fullscreen)  
            m_PrimaryMonitor = glfwGetPrimaryMonitor();
        
        if(m_PrimaryMonitor == nullptr)
            m_Window = glfwCreateWindow(width, height, title, nullptr, nullptr);
        else
            m_Window = glfwCreateWindow(width, height, title, m_PrimaryMonitor, nullptr);

        if (!m_Window) 
        {
            glfwTerminate();
            throw std::runtime_error("Failed to create GLFW window");
        }
       
        glfwMakeContextCurrent(m_Window);
        glfwSwapInterval(1); // Enable vsync
    }

    Window::~Window() 
    {
        glfwDestroyWindow(m_Window);
        glfwTerminate();
    }

}
