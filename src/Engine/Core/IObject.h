/**
 * @brief Interface for objects in the 3D engine.
 * This interface defines the essential properties and behaviors
 * that all objects must implement.
 */

#pragma once

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>

#include <string>
#include "Graphics/Ray.h"
#include "Graphics/Buffers/IndexBuffer.h"
#include "Graphics/Buffers/VertexArray.h"
#include "Graphics/Buffers/VertexBuffer.h"
#include "Graphics/Renderer/Renderer.h"
#include "Graphics/Shader/Shader.h"
#include "Graphics/Renderer/IRenderable.h"
#include "Graphics/Texture.h"


namespace isaacObjectViewer
{
    class IObject : public IRenderable
    {
    public:
        /// @brief Destroys the scene object.
        virtual ~IObject() = default;

        /// @brief Gets the shininess of the material.
        virtual float& GetShininess() = 0;

        /// @brief Sets the diffuse texture of the material.
        virtual void SetDiffuseTexture(const std::shared_ptr<Texture>& tex) = 0;
        
        /// @brief Sets the specular texture of the material.
        virtual void SetSpecularTexture(const std::shared_ptr<Texture>& tex) = 0;

        /// @brief Intersects a ray with the scene object.
        /// @param ray The ray to intersect.
        /// @param outDistance The distance to the intersection point (if not null).
        /// @return True if the ray intersects the object, false otherwise.
        virtual bool IntersectRay(const Ray& ray, float* outDistance) = 0;

        /// @brief Generates a unique ID for the scene object.
        /// @return The unique ID.
        virtual std::size_t GenerateUniqueID() = 0;

        /// @brief Gets the vertex array of the scene object.
        /// @return The vertex array.
        virtual inline const VertexArray    &GetVertexArray() const = 0;

        /// @brief Gets the vertex buffer of the scene object.
        /// @return The vertex buffer.
        virtual inline const VertexBuffer   &GetVertexBuffer() const = 0;

        /// @brief Gets the index buffer of the scene object.
        /// @return The index buffer.
        virtual inline const IndexBuffer    &GetIndexBuffer() const = 0;

        /// @brief Gets the index count of the scene object.
        /// @return The index count.
        virtual inline unsigned int         GetIndexCount() const = 0;

        /// @brief Gets the vertex count of the scene object.
        /// @return The vertex count.
        virtual inline unsigned int         GetVertexCount() const = 0;
    };
}