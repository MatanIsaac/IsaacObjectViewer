#pragma once

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>

#include <string>
#include "Core/Ray.h"
#include "Graphics/Buffers/IndexBuffer.h"
#include "Graphics/Buffers/VertexArray.h"
#include "Graphics/Buffers/VertexBuffer.h"
#include "Graphics/Renderer/Renderer.h"
#include "Graphics/Shaders/Shader.h"
#include "Graphics/Renderer/IRenderable.h"
#include "Graphics/Texture.h"


namespace isaacObjectViewer
{
    class ISceneObject : public IRenderable
    {
    public:
        virtual ~ISceneObject() = default;

        
        virtual float& GetShininess() = 0;

        virtual void SetDiffuseTexture(const std::shared_ptr<Texture>& tex) = 0;
        virtual void SetSpecularTexture(const std::shared_ptr<Texture>& tex) = 0;

        // returns true if hit and sets outdistance to the hit distance (if not null)
        virtual bool IntersectRay(const Ray& ray, float* outDistance) = 0;

        virtual std::size_t GenerateUniqueID() = 0;

        virtual inline const VertexArray    &GetVertexArray() const = 0;
        virtual inline const VertexBuffer   &GetVertexBuffer() const = 0;
        virtual inline const IndexBuffer    &GetIndexBuffer() const = 0;
        virtual inline unsigned int         GetIndexCount() const = 0; 
        virtual inline unsigned int         GetVertexCount() const = 0;

    };
}