#include "Mouse.h"
#include "Ray.h"
#include "Engine.h"
#include "Scene/ISceneObject.h"
#include "Utility/Log.hpp"

namespace isaacObjectViewer
{
    Mouse *Mouse::s_Instance = nullptr;

    void Mouse::ProcessMotion(Camera* camera, float xoffset, float yoffset, bool constrainPitch)
    {
        // scale the raw mouse deltas by your sensitivity setting
        xoffset *= m_Sensitivity;
        yoffset *= m_Sensitivity;

        // apply them to the camera's yaw and pitch angles
        camera->AddYaw(xoffset);
        camera->AddPitch(yoffset);

        // make sure that when pitch is out of bounds, screen doesn't get flipped
        // clamp pitch to [-89°, +89°] to avoid gimbal-lock singularity
        if (constrainPitch)
        {
            if (camera->GetPitch() > 89.0f)
                camera->SetPitch(89.0f);
            if (camera->GetPitch() < -89.0f)
                camera->SetPitch(-89.0f);
        }

        // update Front, Right and Up Vectors using the updated Euler angles
        camera->UpdateVectors();
    }

    
    void Mouse::ProcessZoom(float yoffset, Camera* camera)
    {
        camera->AddZoom(-yoffset);
        if (camera->GetZoom() < 1.0f)
            camera->SetZoom(1.0f);
        if (camera->GetZoom() > 45.0f)
            camera->SetZoom(45.0f);
    }

    void Mouse::ProcessMouseClick(float mouseX, float mouseY, Camera* camera)
    {   
        Ray pickingRay = Ray::ScreenPointToWorldRay(mouseX, mouseY, 
                                                    SCREEN_WIDTH, SCREEN_HEIGHT, 
                                                    camera->GetViewMatrix(), camera->GetProjectionMatrix());

        auto* engine = Engine::GetInstance(); 
       
        ISceneObject* selected = nullptr;
        float minDist = std::numeric_limits<float>::max();
            
        for(auto* obj : engine->GetSceneObjects())
        {
            float dist;
            if (obj->IntersectRay(pickingRay, &dist) && dist < minDist)
            {
                minDist = dist;
                selected = obj;
            }
        }
        
        if (selected)
        {
            engine->SetSelectedObject(selected); // Mark selected
            auto pos = selected->GetPosition();
            //std::cout << "Selected an object at: " << '(' << pos.x << ',' << pos.y << ',' << pos.z << ')' << std::endl;
            LOG_INFO("Selected an object at ({},{},{})",pos.x,pos.y,pos.z);
        }
    }
}