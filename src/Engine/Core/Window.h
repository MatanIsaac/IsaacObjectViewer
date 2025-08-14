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
    class Window 
    {
    public:
        /// @brief Constructs a new Window object.
        /// @param title The title of the window.
        /// @param width The width of the window.
        /// @param height The height of the window.
        /// @param fullscreen Whether to create the window in fullscreen mode.
        Window(const char *title, int width, int height, bool fullscreen = false);

        /// @brief Destroys the Window object.
        ~Window();

        /// @brief Gets the SDL window.
        /// @return The SDL window.
        SDL_Window*     GetSDLWindow()      { return m_Window; }

        /// @brief Gets the OpenGL context.
        /// @return The OpenGL context.
        SDL_GLContext   GetGLContext()      { return m_GL_Context; }
        
        /// @brief Gets the GLSL version.
        /// @return The GLSL version.
        const char*     GetGLSLVersion()    { return glsl_version; }
    private:
        SDL_Window*     m_Window;
        SDL_GLContext   m_GL_Context;
        const char*     glsl_version = "#version 330";
    };
}