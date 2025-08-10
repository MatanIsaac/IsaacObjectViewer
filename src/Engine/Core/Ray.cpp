#include "Ray.h"

namespace isaacObjectViewer
{
    Ray* Ray::s_Instance = nullptr;
    
    Ray Ray::ScreenPointToWorldRay(float mouseX, float mouseY, 
                                    int windowWidth, int windowHeight, 
                                    const glm::mat4 &view, const glm::mat4 &projection, 
                                    float distance, float duration, bool trace)
    {
        // Convert mouse coordinates to NDC
        glm::vec2 mouseNDC;
        mouseNDC.x = (2.0f * mouseX) / windowWidth - 1.0f;
        mouseNDC.y = 1.0f - (2.0f * mouseY) / windowHeight; // OpenGL: Y is flipped

        // Create the ray in NDC space
        glm::vec4 rayStartNDC(mouseNDC,  0.0f, 1.0f); // near plane
        glm::vec4 rayEndNDC  (mouseNDC,  1.0f, 1.0f); // far plane

        // Inverse the view-projection matrix 
        glm::mat4 invVP = glm::inverse(projection * view);

        // Transform the ray start and end points to world space
        glm::vec4 rayStartWorld = invVP * rayStartNDC;
        rayStartWorld /= rayStartWorld.w;

        glm::vec4 rayEndWorld = invVP * rayEndNDC;
        rayEndWorld /= rayEndWorld.w;

        glm::vec3 origin = glm::vec3(rayStartWorld);
        glm::vec3 end = glm::vec3(rayEndWorld);
        glm::vec3 direction = glm::normalize(end - origin);

        // Add the line to the tracer
        if (trace)
        {
            // Pass origin and direction, not end point
            Tracer::GetInstance()->AddLine(origin, direction * distance, duration);
        }
        return Ray(origin, direction);
    }
}