#include "Mouse.h"


namespace isaacGraphicsEngine
{
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
}