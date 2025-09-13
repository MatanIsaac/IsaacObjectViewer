#include "Mouse.h"
#include "Graphics/Ray.h"
#include "Engine.h"
#include "Core/IObject.h"
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
        camera->AddFOV(-yoffset);
        if (camera->GetFOV() < 1.0f)
        {
            camera->SetFOV(1.0f);
        }
        if (camera->GetFOV() > 45.0f)
        {
            camera->SetFOV(45.0f);
        }
    }

    void Mouse::ProcessMouseClick(float mouseX, float mouseY, Camera* camera)
    {   
        auto* engine = Engine::GetInstance(); 

        int fbW, fbH;
        SDL_GetWindowSizeInPixels(engine->GetSDLWindow(), &fbW, &fbH);

        Ray pickingRay = Ray::GetInstance()->ScreenPointToWorldRay(mouseX, mouseY,
            static_cast<float>(fbW), static_cast<float>(fbH),
            camera->GetViewMatrix(), camera->GetProjectionMatrix(), 10.f, 10.f, m_TraceMouseRay);

        IObject* selected = nullptr;
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
            engine->SetSelectedObject(selected);
        }
    }
    

}