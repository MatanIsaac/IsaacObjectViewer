#pragma once

#include <glad/glad.h>
#include <iostream>

// '__' MSVC Only
#define ASSERT(x)                                                                                  \
    if (!(x))                                                                                      \
        __debugbreak();
#define GLCall(x)                                                                                  \
    GLClearError();                                                                                \
    x;                                                                                             \
    ASSERT(GLLogCall(#x, __FILE__, __LINE__))


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
