#pragma once

#include "Utility/config.h"
#include "Utility/Log.hpp"
#include "Window.h"
#include "Scene/Camera/Camera.h"
#include "Scene/ISceneObject.h"
#include "Graphics/OpenGL/Primitives/Sphere.h"
#include "Graphics/OpenGL/Primitives/Plane.h"
#include "Graphics/OpenGL/Primitives/Cube.h"
#include "Graphics/OpenGL/Primitives/Cylinder.h"
#include "Graphics/OpenGL/Lighting/Light.h"
#include "Graphics/OpenGL/Renderer/Renderer.h"
#include "Graphics/OpenGL/Shaders/Shader.h"
#include "../UI/ImGuiLayer.h"

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

        // Scene Objects
        //-----------------------------------------------------------------------
        
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
                case ObjectType::Light:
                    obj = new Light(position,{1.0f,1.0f,1.0f});
                    break;
                default:
                    LOG_INFO("Object Type must be a: Cube/Sphere/Cylinder/Plane/Light\n");
                    break;
            }
            if(obj)
            {
                m_SceneObjects.push_back(obj);
                if (type == ObjectType::Light)
                    m_LightObjects.push_back(static_cast<Light*>(obj));
            }
        }
        
        inline void RemoveSceneObject(ISceneObject* object)
        {
            m_SceneObjects.erase(
                std::remove_if(m_SceneObjects.begin(), m_SceneObjects.end(),
                    [object](ISceneObject* obj)
                    {
                        if(obj && obj->GetID() == object->GetID())
                        {
                            delete obj;
                            return true; 
                        }
                        return false;
                    }),
                    m_SceneObjects.end()
                );
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
        std::vector<Light*> GetLightObjects()
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
        Shader* m_lightingShader;
        
        Camera* m_Camera;
        
        ISceneObject* m_SelectedObject;
        std::vector<ISceneObject*> m_SceneObjects;
        std::vector<Light*> m_LightObjects;
        const int MAX_LIGHTS = 8;

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

        Engine(const Engine &other) = delete;
        Engine &operator=(const Engine &other) = delete;
        Engine(Engine &&other) = delete;
        Engine &operator=(Engine &&other) = delete;
    };


} // namespace isaacGraphicsEngine