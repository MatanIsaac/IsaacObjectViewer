#pragma once
#include "Utility/config.h"
#include "../../../Core/Ray.h"

namespace isaacObjectLoader
{
    class IPrimitive
    {
    public:
        virtual ~IPrimitive() {}
        virtual glm::vec3& GetPosition() = 0;
        virtual glm::vec3& GetRotation() = 0;
        virtual glm::vec3& GetScale() = 0;
        virtual void SetPosition(const glm::vec3& newPosition) = 0;
        virtual void SetRotation(const glm::vec3& newRotation) = 0;
        virtual void SetScale(const glm::vec3& newScale) = 0;
        
        static constexpr glm::vec3 DEFAULT_POSITION = {1.0f, 1.0f, 1.0f};
        static constexpr glm::vec3 DEFAULT_ROTATION = {1.0f, 1.0f, 1.0f};
        static constexpr glm::vec3 DEFAULT_SCALE = {1.0f, 1.0f, 1.0f};
        static constexpr glm::vec3 DEFAULT_COLOR = {1.0f, 0.5f, 0.31f};
        
        // returns true if hit and sets outdistance to the hit distance (if not null)
        virtual bool IntersectRay(const Ray& ray, float* outDistance) const = 0;
    };
}