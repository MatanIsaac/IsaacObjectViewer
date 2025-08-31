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

        /// @brief Intersects a ray with the scene object.
        /// @param ray The ray to intersect.
        /// @param outDistance The distance to the intersection point (if not null).
        /// @return True if the ray intersects the object, false otherwise.
        virtual bool IntersectRay(const Ray& ray, float* outDistance) = 0;

        
    };
}