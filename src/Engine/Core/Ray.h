#pragma once 
#include "Utility/config.h"

namespace isaacObjectLoader
{   
    class Ray
    {
    public:
        Ray(const glm::vec3& origin, const glm::vec3& direction)
            : m_origin(origin), m_direction(glm::normalize(direction)) {}

        // getters 
        const glm::vec3& GetOrigin() const { return m_origin; }
        const glm::vec3& GetDirection() const { return m_direction; }

        static Ray ScreenPointToWorldRay(float mouseX, float mouseY, 
                                        int windowWidth, int windowHeight,
                                        const glm::mat4& view, const glm::mat4& projection)
        {
            glm::vec2 mouseNDC;
            mouseNDC.x = (2.0f * mouseX) / windowWidth - 1.0f;
            mouseNDC.y = 1.0f - (2.0f * mouseY) / windowHeight; // OpenGL: Y is flipped

            glm::vec4 rayStartNDC(mouseNDC, -1.0f, 1.0f); // near plane
            glm::vec4 rayEndNDC  (mouseNDC,  1.0f, 1.0f); // far plane

            glm::mat4 invVP = glm::inverse(projection * view);

            glm::vec4 rayStartWorld = invVP * rayStartNDC;
            rayStartWorld /= rayStartWorld.w;

            glm::vec4 rayEndWorld = invVP * rayEndNDC;
            rayEndWorld /= rayEndWorld.w;

            glm::vec3 origin = glm::vec3(rayStartWorld);
            glm::vec3 direction = glm::normalize(glm::vec3(rayEndWorld - rayStartWorld));

            return Ray(origin, direction);
        }

    private:
        glm::vec3 m_origin;
        glm::vec3 m_direction; // Always normalized!
    };

    // Returns true if hit, and stores hit distance in outDist
    inline bool RayIntersectsAABB(const Ray& ray, const glm::vec3& boxMin, const glm::vec3& boxMax, float* outDist)
    {
        float tMin = (boxMin.x - ray.GetOrigin().x) / ray.GetDirection().x;
        float tMax = (boxMax.x - ray.GetOrigin().x) / ray.GetDirection().x;
        if (tMin > tMax) std::swap(tMin, tMax);

        float tyMin = (boxMin.y - ray.GetOrigin().y) / ray.GetDirection().y;
        float tyMax = (boxMax.y - ray.GetOrigin().y) / ray.GetDirection().y;
        if (tyMin > tyMax) std::swap(tyMin, tyMax);

        if ((tMin > tyMax) || (tyMin > tMax))
            return false;

        if (tyMin > tMin)
            tMin = tyMin;
        if (tyMax < tMax)
            tMax = tyMax;

        float tzMin = (boxMin.z - ray.GetOrigin().z) / ray.GetDirection().z;
        float tzMax = (boxMax.z - ray.GetOrigin().z) / ray.GetDirection().z;
        if (tzMin > tzMax) std::swap(tzMin, tzMax);

        if ((tMin > tzMax) || (tzMin > tMax))
            return false;

        if (tzMin > tMin)
            tMin = tzMin;
        if (tzMax < tMax)
            tMax = tzMax;

        if (tMin < 0) // box is behind camera
            return false;

        if (outDist) *outDist = tMin;
        return true;
    }

}