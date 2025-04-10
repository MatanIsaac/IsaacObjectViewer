#pragma once
#include "Utility/config.h"

namespace isaacGraphicsEngine
{
    class Window 
    {
    public:
        Window(const char *title, int width, int height, bool fullscreen = false);
        ~Window();

        SDL_Window*     GetSDLWindow()      { return m_Window; }
        SDL_GLContext   GetGLContext()      { return m_GL_Context; }
        const char*     GetGLSLVersion()    { return glsl_version; }
    private:
        SDL_Window*     m_Window;
        SDL_GLContext   m_GL_Context;
        const char* glsl_version = "#version 330";
    };
}