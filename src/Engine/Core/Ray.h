#pragma once 
#include "Utility/config.h"
#include "Graphics/Primitives/Tracer.h"

namespace isaacObjectViewer
{   
    class Ray
    {
    public:
        static Ray* GetInstance()
        {
            if (s_Instance == nullptr)
                s_Instance = new Ray({0.f,0.f,0.f}, {0.f,0.f,0.f});
            return s_Instance;
        }

        // getters 
        const glm::vec3& GetOrigin() const { return m_origin; }
        const glm::vec3& GetDirection() const { return m_direction; }
    
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
    
    // Returns true if hit, and stores hit distance in outDist
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