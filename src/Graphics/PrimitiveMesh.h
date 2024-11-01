#pragma once

#include "Texture.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include <memory>

namespace isaacObjectLoader
{
class PrimitiveMesh
{
public:
    virtual ~PrimitiveMesh() = 0;

    // Deleted copy constructor and assignment operator to prevent copying
    PrimitiveMesh(const PrimitiveMesh&)            = delete;
    PrimitiveMesh& operator=(const PrimitiveMesh&) = delete;

    // Update mesh properties (if applicable)
    virtual void Update() = 0;

    // Accessors for texture
    virtual void           SetTexture(std::unique_ptr<Texture> texture) = 0;
    virtual const Texture* GetTexture() const                           = 0;

protected:
    // Constructor as protected for use in derived classes
    PrimitiveMesh() = default;

    std::unique_ptr<VertexArray>  m_VertexArray;
    std::unique_ptr<VertexBuffer> m_VertexBuffer;
    std::unique_ptr<Texture>      m_Texture;
};

inline PrimitiveMesh::~PrimitiveMesh() {}  // Implementation of pure virtual destructor
}
