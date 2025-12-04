#include "Window.h"
#include <SDL3/SDL_video.h>
#include <Utility/Log.hpp>

namespace isaacObjectViewer 
{
    Window::Window(const Config& config)
    {
        auto raw_window = SDL_CreateWindow(config.title.c_str(), config.width, config.height, config.flags);
        if (!raw_window) 
        {
            throw std::runtime_error("Failed to Create an SDL Window.");
        }
        m_Window = UniqueSDLWindow(raw_window);

        SDL_SetWindowFullscreen(m_Window.get(), config.fullscreen);          
       
        m_GL_Context = SDL_GL_CreateContext(m_Window.get());
        if (m_GL_Context == nullptr)
        {
            throw std::runtime_error("Failed to create SDL_GL_CreateContext.");
        }

        SDL_SetWindowPosition(m_Window.get(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
        SDL_GL_MakeCurrent(m_Window.get(), m_GL_Context);
        SDL_GL_SetSwapInterval(1); // Enable vsync
        SDL_SetWindowPosition(m_Window.get(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
        SDL_ShowWindow(m_Window.get());
    }

    Window::~Window() 
    {
        LOG_INFO("Window Destructor Called!...");
        SDL_GL_DestroyContext(m_GL_Context);
    }

    int Window::GetWidth() const 
    {
        int w = 0, h = 0;
        // Always use the handle returned by m_window.get()
        SDL_GetWindowSize(m_Window.get(), &w, &h); 
        return w;
    }

    int Window::GetHeight() const 
    {
        int w = 0, h = 0;
        SDL_GetWindowSize(m_Window.get(), &w, &h);
        return h;
    }

}
