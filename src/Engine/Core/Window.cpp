#include "Window.h"
#include <SDL3/SDL_video.h>

namespace isaacObjectLoader 
{

    Window::Window(const char *title, int width, int height, bool fullscreen)
    {
        SDL_InitFlags init_flags = SDL_INIT_VIDEO;
        if (!SDL_Init(init_flags)) 
        {
            throw std::runtime_error("Failed to initialize SDL.");
        }
        
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

        // Create window with graphics context
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

        SDL_WindowFlags window_flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN;
        m_Window = SDL_CreateWindow(title, width, height, window_flags);
        if (!m_Window) 
        {
            throw std::runtime_error("Failed to Create an SDL Window.");
        }
        SDL_SetWindowFullscreen(m_Window,fullscreen);          
       
        m_GL_Context = SDL_GL_CreateContext(m_Window);
        if (m_GL_Context == nullptr)
        {
            throw std::runtime_error("Failed to create SDL_GL_CreateContext.");
        }

        SDL_SetWindowPosition(m_Window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
        SDL_GL_MakeCurrent(m_Window, m_GL_Context);
        SDL_GL_SetSwapInterval(1); // Enable vsync
        SDL_SetWindowPosition(m_Window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
        SDL_ShowWindow(m_Window);
    }

    Window::~Window() 
    {
        SDL_GL_DestroyContext(m_GL_Context);
        SDL_DestroyWindow(m_Window);
        m_Window = nullptr;
    }
}
