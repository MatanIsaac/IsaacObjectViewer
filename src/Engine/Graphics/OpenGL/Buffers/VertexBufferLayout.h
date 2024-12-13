#pragma once
#include "Utility/config.h"
#include "Utility/GLErrorManager.h"

namespace isaacGraphicsEngine
{

struct VertexBufferElement
{
    unsigned int m_Count;
    unsigned int m_Type;
    bool         m_Normalized;

    static unsigned int GetSizeOfType(unsigned int type)
    {
        switch (type)
        {
            case GL_FLOAT:
                return 4;
            case GL_UNSIGNED_INT:
                return 4;
            case GL_UNSIGNED_BYTE:
                return 1;
        }
        ASSERT(false);
        return 0;
    }
};

class VertexBufferLayout
{
public:
    VertexBufferLayout();

    template<typename T>
    void Push(unsigned int count);

    inline unsigned int                            GetStride() const { return m_Stride; }
    inline const std::vector<VertexBufferElement>& GetElements() const { return m_Elements; }

private:
    std::vector<VertexBufferElement> m_Elements;
    unsigned int                     m_Stride;
};


}
