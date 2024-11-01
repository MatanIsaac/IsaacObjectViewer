#pragma once

#include <memory>

#include "IndexBuffer.h"
#include "PrimitiveMesh.h"
#include "Texture.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"

namespace isaacObjectLoader
{
class Cube : public PrimitiveMesh
{
public:
    Cube();
    virtual ~Cube() override;  // Ensure the destructor overrides the base class destructor

    virtual void Update() override;

    // Implement other necessary virtual functions from PrimitiveMesh
    virtual void           SetTexture(std::unique_ptr<Texture> newTexture) override;
    virtual const Texture* GetTexture() const override;

    inline const VertexArray&  GetCubeVA() const { return *m_VertexArray; }
    inline const VertexBuffer& GetCubeVB() const { return *m_VertexBuffer; }
    inline const IndexBuffer&  GetCubeIB() const { return *m_IndexBuffer; }
    inline int                 GetVertexCount() const { return m_VertexCount; }

private:
    std::unique_ptr<IndexBuffer>  m_IndexBuffer;
    std::unique_ptr<VertexBuffer> m_VertexBuffer;
    std::unique_ptr<VertexArray>  m_VertexArray;
    int                           m_VertexCount;
    int                           m_IndicesCount;
    std::unique_ptr<Texture>      m_Texture;

public:
    static constexpr float m_CubeVertices[288] = { // positions        // texture coords // normals
                                                   // Front face
                                                   -0.5f,
                                                   -0.5f,
                                                   -0.5f,
                                                   0.0f,
                                                   0.0f,
                                                   -1.0f,
                                                   0.5f,
                                                   -0.5f,
                                                   -0.5f,
                                                   0.0f,
                                                   0.0f,
                                                   -1.0f,
                                                   0.5f,
                                                   0.5f,
                                                   -0.5f,
                                                   0.0f,
                                                   0.0f,
                                                   -1.0f,
                                                   0.5f,
                                                   0.5f,
                                                   -0.5f,
                                                   0.0f,
                                                   0.0f,
                                                   -1.0f,
                                                   -0.5f,
                                                   0.5f,
                                                   -0.5f,
                                                   0.0f,
                                                   0.0f,
                                                   -1.0f,
                                                   -0.5f,
                                                   -0.5f,
                                                   -0.5f,
                                                   0.0f,
                                                   0.0f,
                                                   -1.0f,

                                                   // Back face
                                                   -0.5f,
                                                   -0.5f,
                                                   0.5f,
                                                   0.0f,
                                                   0.0f,
                                                   1.0f,
                                                   0.5f,
                                                   -0.5f,
                                                   0.5f,
                                                   0.0f,
                                                   0.0f,
                                                   1.0f,
                                                   0.5f,
                                                   0.5f,
                                                   0.5f,
                                                   0.0f,
                                                   0.0f,
                                                   1.0f,
                                                   0.5f,
                                                   0.5f,
                                                   0.5f,
                                                   0.0f,
                                                   0.0f,
                                                   1.0f,
                                                   -0.5f,
                                                   0.5f,
                                                   0.5f,
                                                   0.0f,
                                                   0.0f,
                                                   1.0f,
                                                   -0.5f,
                                                   -0.5f,
                                                   0.5f,
                                                   0.0f,
                                                   0.0f,
                                                   1.0f,

                                                   // Left face
                                                   -0.5f,
                                                   0.5f,
                                                   0.5f,
                                                   -1.0f,
                                                   0.0f,
                                                   0.0f,
                                                   -0.5f,
                                                   0.5f,
                                                   -0.5f,
                                                   -1.0f,
                                                   0.0f,
                                                   0.0f,
                                                   -0.5f,
                                                   -0.5f,
                                                   -0.5f,
                                                   -1.0f,
                                                   0.0f,
                                                   0.0f,
                                                   -0.5f,
                                                   -0.5f,
                                                   -0.5f,
                                                   -1.0f,
                                                   0.0f,
                                                   0.0f,
                                                   -0.5f,
                                                   -0.5f,
                                                   0.5f,
                                                   -1.0f,
                                                   0.0f,
                                                   0.0f,
                                                   -0.5f,
                                                   0.5f,
                                                   0.5f,
                                                   -1.0f,
                                                   0.0f,
                                                   0.0f,

                                                   // Right face
                                                   0.5f,
                                                   0.5f,
                                                   0.5f,
                                                   1.0f,
                                                   0.0f,
                                                   0.0f,
                                                   0.5f,
                                                   0.5f,
                                                   -0.5f,
                                                   1.0f,
                                                   0.0f,
                                                   0.0f,
                                                   0.5f,
                                                   -0.5f,
                                                   -0.5f,
                                                   1.0f,
                                                   0.0f,
                                                   0.0f,
                                                   0.5f,
                                                   -0.5f,
                                                   -0.5f,
                                                   1.0f,
                                                   0.0f,
                                                   0.0f,
                                                   0.5f,
                                                   -0.5f,
                                                   0.5f,
                                                   1.0f,
                                                   0.0f,
                                                   0.0f,
                                                   0.5f,
                                                   0.5f,
                                                   0.5f,
                                                   1.0f,
                                                   0.0f,
                                                   0.0f,

                                                   // Bottom face
                                                   -0.5f,
                                                   -0.5f,
                                                   -0.5f,
                                                   0.0f,
                                                   -1.0f,
                                                   0.0f,
                                                   0.5f,
                                                   -0.5f,
                                                   -0.5f,
                                                   0.0f,
                                                   -1.0f,
                                                   0.0f,
                                                   0.5f,
                                                   -0.5f,
                                                   0.5f,
                                                   0.0f,
                                                   -1.0f,
                                                   0.0f,
                                                   0.5f,
                                                   -0.5f,
                                                   0.5f,
                                                   0.0f,
                                                   -1.0f,
                                                   0.0f,
                                                   -0.5f,
                                                   -0.5f,
                                                   0.5f,
                                                   0.0f,
                                                   -1.0f,
                                                   0.0f,
                                                   -0.5f,
                                                   -0.5f,
                                                   -0.5f,
                                                   0.0f,
                                                   -1.0f,
                                                   0.0f,

                                                   // Top face
                                                   -0.5f,
                                                   0.5f,
                                                   -0.5f,
                                                   0.0f,
                                                   1.0f,
                                                   0.0f,
                                                   0.5f,
                                                   0.5f,
                                                   -0.5f,
                                                   0.0f,
                                                   1.0f,
                                                   0.0f,
                                                   0.5f,
                                                   0.5f,
                                                   0.5f,
                                                   0.0f,
                                                   1.0f,
                                                   0.0f,
                                                   0.5f,
                                                   0.5f,
                                                   0.5f,
                                                   0.0f,
                                                   1.0f,
                                                   0.0f,
                                                   -0.5f,
                                                   0.5f,
                                                   0.5f,
                                                   0.0f,
                                                   1.0f,
                                                   0.0f,
                                                   -0.5f,
                                                   0.5f,
                                                   -0.5f,
                                                   0.0f,
                                                   1.0f,
                                                   0.0f

    };
};
}  // namespace isaacObjectLoader
