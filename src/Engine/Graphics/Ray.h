/**
 * @file Ray.h
 * @brief Header file for the Ray class.
 *  This class represents a ray in 3D space, defined by an origin and a direction.
 */

#pragma once 
#include "Utility/config.h"
#include "Graphics/Tracer.h"

namespace isaacObjectViewer
{   
    class Ray
    {
    public:
        /// @brief Gets the singleton instance of the Ray class.
        /// @return The singleton instance of the Ray class.
        static Ray* GetInstance()
        {
            if (s_Instance == nullptr)
                s_Instance = new Ray({0.f,0.f,0.f}, {0.f,0.f,0.f});
            return s_Instance;
        }

        /// @brief Gets the origin of the ray.
        /// @return The origin of the ray.
        const glm::vec3& GetOrigin() const { return m_origin; }

        /// @brief Gets the direction of the ray.
        /// @return The direction of the ray.
        const glm::vec3& GetDirection() const { return m_direction; }

        /// @brief Creates a ray from screen space coordinates to world space.
        /// @param mouseX The X coordinate of the mouse in screen space.
        /// @param mouseY The Y coordinate of the mouse in screen space.
        /// @param windowWidth The width of the window.
        /// @param windowHeight The height of the window.
        /// @param view The view matrix.
        /// @param projection The projection matrix.
        /// @param distance The distance to the near plane.
        /// @param duration The duration of the ray.
        /// @param trace Whether to trace the ray.
        /// @return A ray representing the world space direction.
        Ray ScreenPointToWorldRay(float mouseX, float mouseY, 
            int windowWidth, int windowHeight,
            const glm::mat4& view, const glm::mat4& projection, 
            float distance, float duration, bool trace = false);


    private:
        Ray(const glm::vec3& origin, const glm::vec3& direction)
            : m_origin(origin), m_direction(glm::normalize(direction)) {}

        static Ray* s_Instance;
        glm::vec3 m_origin;
        glm::vec3 m_direction;
    };

    /// @brief Checks if a ray intersects with an axis-aligned bounding box (AABB).
    /// @param ray The ray to test for intersection.
    /// @param boxMin The minimum corner of the AABB.
    /// @param boxMax The maximum corner of the AABB.
    /// @param outDist The distance to the intersection point, if it exists.
    /// @return True if the ray intersects the AABB, false otherwise.
    inline bool RayIntersectsAABB(const Ray& ray, const glm::vec3& boxMin, const glm::vec3& boxMax, float* outDist)
    {
        float tMin = (boxMin.x - ray.GetOrigin().x) / ray.GetDirection().x;
        float tMax = (boxMax.x - ray.GetOrigin().x) / ray.GetDirection().x;
        if (tMin > tMax) 
            std::swap(tMin, tMax);

        float tyMin = (boxMin.y - ray.GetOrigin().y) / ray.GetDirection().y;
        float tyMax = (boxMax.y - ray.GetOrigin().y) / ray.GetDirection().y;
        if (tyMin > tyMax) 
            std::swap(tyMin, tyMax);

        if ((tMin > tyMax) || (tyMin > tMax))
            return false;

        if (tyMin > tMin)
            tMin = tyMin;
        if (tyMax < tMax)
            tMax = tyMax;

        float tzMin = (boxMin.z - ray.GetOrigin().z) / ray.GetDirection().z;
        float tzMax = (boxMax.z - ray.GetOrigin().z) / ray.GetDirection().z;
        if (tzMin > tzMax) 
            std::swap(tzMin, tzMax);

        if ((tMin > tzMax) || (tzMin > tMax))
            return false;

        if (tzMin > tMin)
            tMin = tzMin;
        if (tzMax < tMax)
            tMax = tzMax;

        if (tMin < 0) // box is behind camera
            return false;

        if (outDist) 
            *outDist = tMin;
        return true;
    }
}