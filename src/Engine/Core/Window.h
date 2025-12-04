/**
 * @file Window.h
 * @brief Header file for the Window class.
 *  This class represents a window in the application.
 *  It handles the creation, management, and destruction of the window.
 *  As well as the OpenGL context, with the integration of SDL3.
 */

#pragma once
#include "Utility/config.h"

namespace isaacObjectViewer
{
    // Custom deleter for std::unique_ptr
    struct SDLWindowDeleter 
    {
        void operator()(SDL_Window* window) const 
        {
            if (window) 
            {
                // Ensure the correct SDL function is called for cleanup
                SDL_DestroyWindow(window); 
            }
        }
    };
    using UniqueSDLWindow = std::unique_ptr<SDL_Window, SDLWindowDeleter>;

    class Window 
    {
    public:

        // Window configuration structure
        struct Config 
        {
            std::string title = "My Application";
            int width = 1280;
            int height = 720;
            bool fullscreen = false;
            SDL_WindowFlags flags = SDL_WINDOW_RESIZABLE;
        };

        /// @brief Constructs a new Window object.
        /// @param title The title of the window.
        /// @param width The width of the window.
        /// @param height The height of the window.
        /// @param fullscreen Whether to create the window in fullscreen mode.
        Window(const Config& config);

        /// @brief Destroys the Window object.
        ~Window();

        // Window objects should NOT be copied, only moved if needed. (handled by unique_ptr)
        Window(const Window&) = delete;
        Window& operator=(const Window&) = delete;
        Window(Window&&) = default;
        Window& operator=(Window&&) = default;

        /// @brief Gets the SDL window.
        /// @return The SDL window.
        SDL_Window*     GetSDLWindow()      { return m_Window.get(); }

        /// @brief Gets the OpenGL context.
        /// @return The OpenGL context.
        SDL_GLContext   GetGLContext()      { return m_GL_Context; }
        
        /// @brief Gets the GLSL version.
        /// @return The GLSL version.
        const char*     GetGLSLVersion()    { return glsl_version; }

        /// @brief Gets the width of the window.
        /// @return The width of the window.
        int GetWidth() const;
        
        /// @brief Gets the height of the window.
        /// @return The height of the window.
        int GetHeight() const;
    private:
        UniqueSDLWindow m_Window;
        SDL_GLContext   m_GL_Context;
        const char*     glsl_version = "#version 330";
    };
}