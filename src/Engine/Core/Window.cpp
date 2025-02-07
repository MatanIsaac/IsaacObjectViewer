#include "Window.h"

namespace isaacGraphicsEngine 
{

    Window::Window(const char *title, int width, int height, bool fullscreen)
    {
        if (!glfwInit()) 
        {
            throw std::runtime_error("Failed to initialize GLFW");
        }
        
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
          
        m_PrimaryMonitor = glfwGetPrimaryMonitor();
        
        if(!fullscreen)
            m_Window = glfwCreateWindow(width, height, title, nullptr, nullptr);
        else
            m_Window = glfwCreateWindow(width, height, title, m_PrimaryMonitor, nullptr);
    
        if (!m_Window) 
        {
            glfwTerminate();
            throw std::runtime_error("Failed to create GLFW window");
        }
       
        const GLFWvidmode* mode = glfwGetVideoMode(m_PrimaryMonitor);
        // Putting it in the centre
        glfwSetWindowPos(m_Window, mode->width/6, mode->height/6);
        

        glfwMakeContextCurrent(m_Window);
        glfwSwapInterval(0); 
    }

    Window::~Window() 
    {
        glfwDestroyWindow(m_Window);
        glfwTerminate();
    }

}
