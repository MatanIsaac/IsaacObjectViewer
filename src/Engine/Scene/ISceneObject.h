#pragma once

#include <glm/glm.hpp>
#include <string>
#include "Core/Ray.h"

namespace isaacObjectLoader
{
    class ISceneObject
    {
    public:
        virtual ~ISceneObject() = default;

        virtual std::size_t GetID() const = 0;
        virtual const std::string& GetName() const = 0;
        virtual void SetName(const std::string& name) = 0;

        virtual glm::vec3& GetPosition() = 0;
        virtual glm::vec3& GetRotation() = 0;
        virtual glm::vec3& GetScale() = 0;
        virtual void SetPosition(const glm::vec3& newPosition) = 0;
        virtual void SetRotation(const glm::vec3& newRotation) = 0;
        virtual void SetScale(const glm::vec3& newScale) = 0;
        
        static constexpr glm::vec3 DEFAULT_POSITION = {0.0f, 0.0f, 0.0f};
        static constexpr glm::vec3 DEFAULT_ROTATION = {0.0f, 0.0f, 0.0f};
        static constexpr glm::vec3 DEFAULT_SCALE = {1.0f, 1.0f, 1.0f};
        static constexpr glm::vec3 DEFAULT_COLOR = {1.0f, 0.5f, 0.31f};

        // returns true if hit and sets outdistance to the hit distance (if not null)
        virtual bool IntersectRay(const Ray& ray, float* outDistance) const = 0;

        static inline std::size_t GenerateUniqueID()
        {
            static std::size_t currentID = 0;
            return ++currentID;
        }
    };
}