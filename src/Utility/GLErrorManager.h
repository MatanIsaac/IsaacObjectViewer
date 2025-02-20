#pragma once

#include <glad/glad.h>
#include <iostream>

#ifdef _WIN32
    // MSVC only: Use '__debugbreak()' for debugging
    #define ASSERT(x) if (!(x)) __debugbreak();
    #define GLCall(x) do { GLClearError(); x; ASSERT(GLLogCall(#x, __FILE__, __LINE__)); } while(0)
#else
    // Use '__builtin_trap()' for GCC/Clang or 'abort()' as a fallback
    #include <cstdlib>
    #define ASSERT(x) if (!(x)) __builtin_trap();
    #define GLCall(x) do { GLClearError(); x; ASSERT(GLLogCall(#x, __FILE__, __LINE__)); } while(0)
#endif

inline void GLClearError()
{
    while (glGetError() != GL_NO_ERROR)
        ;
}

inline bool GLLogCall(const char* function, const char* file, int line)
{
    while (GLenum error = glGetError())
    {
        std::cout << "[OpenGL Error] (" << error << "): " << function << " " << file << ":" << line
                  << std::endl;

        switch (error)
        {
            case GL_INVALID_ENUM:
                std::cout << "GL_INVALID_ENUM\n";
                break;
            case GL_INVALID_VALUE:
                std::cout << "GL_INVALID_VALUE\n";
                break;
            case GL_INVALID_OPERATION:
                std::cout << "GL_INVALID_OPERATION\n";
                break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                std::cout << "GL_INVALID_FRAMEBUFFER_OPERATION\n";
                break;
            case GL_OUT_OF_MEMORY:
                std::cout << "GL_OUT_OF_MEMORY\n";
                break;
            // case GL_STACK_UNDERFLOW: std::cout << "GL_STACK_UNDERFLOW\n";
            //	break;
            // case GL_STACK_OVERFLOW: std::cout << "GL_STACK_OVERFLOW\n";
            //	break;
            default:
                std::cout << "ERROR DEFAULT\n";
                break;
        }
        return false;
    }
    return true;
}
