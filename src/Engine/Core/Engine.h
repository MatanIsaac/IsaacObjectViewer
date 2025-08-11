#pragma once

#include "Utility/config.h"
#include "Utility/Log.hpp"
#include "Window.h"
#include "Scene/Camera/Camera.h"
#include "Scene/ISceneObject.h"
#include "Graphics/Primitives/Sphere.h"
#include "Graphics/Primitives/Plane.h"
#include "Graphics/Primitives/Cube.h"
#include "Graphics/Primitives/Cylinder.h"
#include "Graphics/Lighting/PointLight.h"
#include "Graphics/Lighting/DirectionalLight.h"
#include "Graphics/Renderer/Renderer.h"
#include "Graphics/Shaders/Shader.h"
#include "../UI/ImGuiLayer.h"
#include "Utility/Timer.h"

namespace isaacObjectViewer
{
    class Engine
    {
    public:
        static Engine *GetInstance()
        {
            if (s_Instance == nullptr)
            {
                s_Instance = new Engine();
            }
            return s_Instance;
        }

        // @brief Starts the engine.
        void Run(bool fullscreen = false);
        inline void Exit() { m_IsRunning = false; }

        // @brief initializes the engine's dependencies, resources and engine objects
        bool Init(const char *title, int width, int height, bool fullscreen = false);

        // @brief processes user input
        void ProcessInput();

        // @brief updates all of the engine dependencies, resources and engine objects.
        void Update(float dt);

        // @brief renders all of the engine textures, sounds and engine objects.
        void Render();

        // @brief calls destructor, which cleans all of the engine resources.
        void Clean();

        // @brief loads up needed resources such as textures, sfx, music etc..
        bool LoadResources();

        Camera *GetCamera() { return m_Camera; }
        
        DirectionalLight& GetDirectionalLight() const { return *m_DirLight; }

        SDL_Window* GetSDLWindow() 
        { return m_Window->GetSDLWindow(); }

        bool GetDisableInput() { return m_MouseModeEnabled; }
        bool GetKeyPressed() { return m_KeyPressed; }
        void SetDisableInput(bool disable) { m_MouseModeEnabled = disable; }
        void SetKeyPressed(bool pressed) { m_KeyPressed = pressed; }
        
        float GetFPS() { return m_FPS; }
        glm::vec3& GetBackgroundColor() { return m_BackgroundColor; }
        void SetBackgroundColor(glm::vec3 newColor) { m_BackgroundColor = newColor; }

        void EnableMouseMode();
        void EnableFreeCameraMode();
        
        ImGuiLayer& GetImGuiLayer() { return m_ImGuiLayer; }

        bool& IsVSyncEanbled() { return m_VSyncEnabled; }
        bool& GetFrameCapEnabled() { return m_FrameCapEnabled; }
        int& GetFrameCapFps() { return m_FrameCapFps; }
        inline void ApplySwapInterval()
        {
            SDL_GL_SetSwapInterval(m_VSyncEnabled ? 1 : 0);
        }


        // Scene Objects
        //-----------------------------------------------------------------------
        inline void AddSceneObject(ISceneObject* obj)
        {
            if (!obj) 
            {
                LOG_ERROR("Can't add object, obj is nullptr.");
                return;
            }
            
            m_SceneObjects.push_back(obj);
            if (obj->GetType() == ObjectType::PointLight)
                m_LightObjects.push_back(static_cast<PointLight*>(obj));
        }
        inline void AddSceneObject(ObjectType type,const glm::vec3& position = {0.0f, 0.0f, 0.0f})
        {
            ISceneObject* obj = nullptr;
            switch(type)
            {
                case ObjectType::Cube:
                    obj = new Cube(position);
                    break;
                case ObjectType::Sphere:
                    obj = new Sphere(position);
                    break;
                case ObjectType::Cylinder:
                    obj = new Cylinder(position);
                    break;
                case ObjectType::Plane:
                    obj = new Plane(position);
                    break;
                case ObjectType::PointLight:
                    obj = new PointLight(position,{1.0f,1.0f,1.0f});
                    break;
                default:
                    LOG_INFO("Object Type must be a: Cube/Sphere/Cylinder/Plane/PointLight\n");
                    break;
            }
            if(obj)
            {
                m_SceneObjects.push_back(obj);
                if (type == ObjectType::PointLight)
                    m_LightObjects.push_back(static_cast<PointLight*>(obj));
            }
        }
        
        inline void RemoveSceneObject(ISceneObject* object)
        {
            if (!object) 
            {
                LOG_ERROR("Can't Remove nullptr Object.");
                return;                     
            }

            if (m_SelectedObject == object)
                m_SelectedObject = nullptr;
            m_ImGuiLayer.ResetSelectedObject();

            // If it’s a light, prune that list too
            if (object->GetType() == ObjectType::PointLight)
                m_LightObjects.erase(std::remove(m_LightObjects.begin(),
                                                m_LightObjects.end(),
                                                static_cast<PointLight*>(object)),
                                    m_LightObjects.end());

            // Remove pointer from the main list
            m_SceneObjects.erase(std::remove(m_SceneObjects.begin(),
                                            m_SceneObjects.end(),
                                            object),
                                m_SceneObjects.end());

            delete object;                           // finally free the memory
        }
        
        inline void ClearSceneObjects()
        {
            for (auto obj : m_SceneObjects)
            {
                if(obj != nullptr)
                    delete obj;
            }
            m_SceneObjects.clear();
        }
        std::vector<ISceneObject*>& GetSceneObjects() { return m_SceneObjects; }
        std::vector<PointLight*> GetLightObjects()
        {
            return m_LightObjects;
        }

        ISceneObject* GetSelectedObject() { return m_SelectedObject; }
        void SetSelectedObject(ISceneObject* obj) { m_SelectedObject = obj; }
        
        void SendAllLightsToShader();
        //-----------------------------------------------------------------------
    private:
        Engine();
        
    private:
        static Engine* s_Instance;
        Window* m_Window;
        Shader* m_Shader;
        Shader* m_MainShader;
        
        Camera* m_Camera;
        
        ISceneObject* m_SelectedObject;
        std::vector<ISceneObject*> m_SceneObjects;
        std::vector<PointLight*> m_LightObjects;
        const int MAX_LIGHTS = 8;
        DirectionalLight* m_DirLight;

        Renderer m_Renderer;

        ImGuiLayer m_ImGuiLayer;

        bool m_MouseModeEnabled;
        bool m_FreeCameraModeEnabled;
        bool m_KeyPressed;
        bool m_IsRunning;

        float m_DeltaTime = 0.0f; // time between current frame and last frame
        float m_LastFrame = 0.0f;
        float m_FPS = 0.0f; // Stores the calculated FPS

        glm::vec3 m_BackgroundColor;
        
        bool m_VSyncEnabled = true;
        bool m_FrameCapEnabled = false; // only used when VSync is OFF
        int  m_FrameCapFps = 60;
        isaacObjectViewer::Timer m_FrameTimer;  

        Engine(const Engine &other) = delete;
        Engine &operator=(const Engine &other) = delete;
        Engine(Engine &&other) = delete;
        Engine &operator=(Engine &&other) = delete;
    };


} // namespace isaacGraphicsEngine