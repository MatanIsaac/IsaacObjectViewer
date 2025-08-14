/**
 * @file Engine.h
 * @brief Core runtime for isaacObjectViewer.
 *
 * The Engine is a lazily constructed singleton that initializes the window/GL context,
 * runs the main loop (input → update → render), and provides access to the camera,
 * lighting, renderer, and ImGui layer.
 *
 * Responsibilities:
 * - Windowing, VSync/swap-interval control, optional frame cap, and FPS tracking.
 * - Scene management:
 *   add/remove/clear objects of type IObject, selection handling, and a synced cache of PointLight.
 * - Lighting: one DirectionalLight plus up to MAX_LIGHTS point lights; uploads light data to shaders.
 * - Shading: switches between None/Phong/Blinn-Phong and updates the main shader uniform accordingly.
 *   Note: If none shading is selected, the engine will use the default Blinn-Phong.
 * - Rendering: handles the rendering pipeline, including object submission and draw calls.
 * - Input modes: mouse capture (disable input) and free-camera toggles.
 *
 * Ownership & Lifecycle:
 * - Not copyable/movable; not thread-safe.
 * - AddSceneObject assumes ownership of heap objects and deletes them on removal/clear.
 *   Light list stays consistent with the scene list.
 *
 * Notes:
 * - Getters often return raw pointers or mutable references; do not delete returned pointers.
 * - Mutating exposed flags (e.g., VSync/frame-cap) affects runtime behavior immediately.
 * - Ensure resources/shaders are initialized before changing shading or sending lights.
 */
#pragma once


#include "Utility/config.h"
#include "Utility/Log.hpp"
#include "Window.h"
#include "Camera/Camera.h"
#include "Core/IObject.h"
#include "Graphics/Primitives/Sphere.h"
#include "Graphics/Primitives/Plane.h"
#include "Graphics/Primitives/Cube.h"
#include "Graphics/Primitives/Cylinder.h"
#include "Graphics/Lighting/PointLight.h"
#include "Graphics/Lighting/DirectionalLight.h"
#include "Graphics/Renderer/Renderer.h"
#include "Graphics/Shader/Shader.h"
#include "../UI/ImGuiLayer.h"
#include "Utility/Timer.h"

namespace isaacObjectViewer
{
    /// @brief Shading model types
    enum class ShadingType
    {
        NONE,
        PHONG,
        BLINNPHONG
    };

    class Engine
    {
    public:
        /// @brief Gets the singleton instance of the Engine.
        static Engine *GetInstance()
        {
            if (s_Instance == nullptr)
            {
                s_Instance = new Engine();
            }
            return s_Instance;
        }

        /// @brief Starts the engine
        /// @param fullscreen Whether to start in fullscreen mode.
        void Run(bool fullscreen = false);
        
        /// @brief Stops the engine.
        inline void Exit() { m_IsRunning = false; }

        /// @brief Gets the main camera.
        /// @return The main camera.
        Camera *GetCamera() { return m_Camera; }

        /// @brief Gets the directional light.
        /// @return The directional light.
        DirectionalLight& GetDirectionalLight() const { return *m_DirLight; }
        
        /// @brief Gets the SDL window.
        /// @return The SDL window.
        SDL_Window* GetSDLWindow() { return m_Window->GetSDLWindow(); }
        
        /// @brief Gets the mouse mode state.
        /// @return The mouse mode state.
        bool GetDisableInput() { return m_MouseModeEnabled; }
        
        /// @brief Gets the key pressed state.
        /// @return The key pressed state.
        bool GetKeyPressed() { return m_KeyPressed; }
    
        /// @brief Gets the frames per second.
        /// @return The frames per second.
        float GetFPS() { return m_FPS; }
        
        /// @brief Gets the background color.
        /// @return The background color.
        glm::vec3& GetBackgroundColor() { return m_BackgroundColor; }
        
        /// @brief Gets the ImGui layer.
        /// @return The ImGui layer.
        ImGuiLayer& GetImGuiLayer() { return m_ImGuiLayer; }
        
        /// @brief Gets the frame cap enabled state.
        /// @return The frame cap enabled state.
        bool& GetFrameCapEnabled() { return m_FrameCapEnabled; }
        
        /// @brief Gets the frame cap FPS.
        /// @return The frame cap FPS.
        int& GetFrameCapFps() { return m_FrameCapFps; }

        /// @brief Gets the mouse mode state.
        /// @return The mouse mode state.
        bool& IsMouseModeEnabled() { return m_MouseModeEnabled; }

        /// @brief Gets the free camera mode state.
        /// @return The free camera mode state.
        bool& IsFreeCameraModeEnabled() { return m_FreeCameraModeEnabled; }

        /// @brief Sets the mouse mode state.
        /// @param disable The new mouse mode state.
        void SetDisableInput(bool disable) { m_MouseModeEnabled = disable; }
        
        /// @brief Sets the key pressed state.
        /// @param pressed The new key pressed state.
        void SetKeyPressed(bool pressed) { m_KeyPressed = pressed; }
        
        /// @brief Sets the background color.
        /// @param newColor The new background color.
        void SetBackgroundColor(glm::vec3 newColor) { m_BackgroundColor = newColor; }

        /// @brief Enables the mouse mode.
        void EnableMouseMode();
        
        /// @brief Enables the free camera mode.
        void EnableFreeCameraMode();

        /// @brief Checks if VSync is enabled.
        bool& IsVSyncEnabled() { return m_VSyncEnabled; }
        
        /// @brief Applies the swap interval.
        inline void ApplySwapInterval()
        {
            SDL_GL_SetSwapInterval(m_VSyncEnabled ? 1 : 0);
        }


        // Scene Objects
        //-----------------------------------------------------------------------
        /// @brief Adds a scene object to the engine.
        /// @param type The type of the object to add.
        /// @param position The position of the object.
        /// @note if its a light, it will be added to the light object list as well.
        inline void AddSceneObject(IObject* obj)
        {
            if (!obj) 
            {
                LOG_ERROR("Can't add object, obj is nullptr.");
                return;
            }
            
            m_SelectedObject = obj;
            m_SceneObjects.push_back(obj);
            if (obj->GetType() == ObjectType::PointLight)
                m_LightObjects.push_back(static_cast<PointLight*>(obj));
        }

        /// @brief Adds a scene object to the engine.
        /// @param type The type of the object to add.
        /// @param position The position of the object.
        /// @note if its a light, it will be added to the light object list as well.
        inline void AddSceneObject(ObjectType type,const glm::vec3& position = {0.0f, 0.0f, 0.0f})
        {
            IObject* obj = nullptr;
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
                m_SelectedObject = obj;
                m_SceneObjects.push_back(obj);
                if (type == ObjectType::PointLight)
                    m_LightObjects.push_back(static_cast<PointLight*>(obj));
            }
        }

        /// @brief Removes a scene object from the engine.
        /// @param object The object to remove.
        /// @note if its a light, it will be removed from the light object list as well.
        inline void RemoveSceneObject(IObject* object)
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

        /// @brief Clears all scene objects from the engine.
        inline void ClearSceneObjects()
        {
            for (auto obj : m_SceneObjects)
            {
                if(obj != nullptr)
                    delete obj;
            }
            m_SceneObjects.clear();
        }

        /// @brief Gets all scene objects in the engine.
        /// @return A vector of pointers to all scene objects.
        std::vector<IObject*>& GetSceneObjects() { return m_SceneObjects; }
        std::vector<PointLight*> GetLightObjects()
        {
            return m_LightObjects;
        }

        /// @brief Gets the currently selected scene object.
        /// @return A pointer to the currently selected scene object.
        IObject* GetSelectedObject() { return m_SelectedObject; }
        
        /// @brief Sets the currently selected scene object.
        /// @param obj The object to select.
        void SetSelectedObject(IObject* obj) { m_SelectedObject = obj; }

        // Light
        /// @brief Sends all light objects to the shader.
        void SendAllLightsToShader();
        
        /// @brief Gets the Blinn-Phong shading state.
        /// @return A reference to the Blinn-Phong shading state.
        bool& GetBlinnPhongShading() { return m_BlinnPhongShading; }
        
        /// @brief Sets the Blinn-Phong shading state.
        /// @param value The new Blinn-Phong shading state.
        void SetBlinnPhongShading(bool value) { m_BlinnPhongShading = value; }
        /// @brief Sets the shading type.
        /// @param type The new shading type.
        void SetShading(ShadingType type) 
        { 
            switch(type)
            {
                case ShadingType::NONE:
                    m_BlinnPhongShading = false;
                    break;
                case ShadingType::PHONG:
                    m_BlinnPhongShading = false;
                    break;
                case ShadingType::BLINNPHONG:
                    m_BlinnPhongShading = true;
                    break;
            }
            m_MainShader->Bind(); 
            m_MainShader->setBool("useBlinnPhong", m_BlinnPhongShading); 
        }
        //-----------------------------------------------------------------------

    private:
        Engine();

        // @brief initializes the engine's dependencies, resources and engine objects
        /// @param title The title of the window.
        /// @param width The width of the window.
        /// @param height The height of the window.
        /// @param fullscreen Whether to start in fullscreen mode.
        /// @return True if initialization was successful, false otherwise.
        bool Init(const char *title, int width, int height, bool fullscreen = false);

        /// @brief processes user input
        void ProcessInput();

        /// @brief updates all of the engine dependencies, resources and engine objects.
        /// @param dt The delta time.
        void Update(float dt);

        /// @brief renders all of the engine textures, sounds and engine objects.
        void Render();

        /// @brief calls destructor, which cleans all of the engine resources.
        void Clean();

        /// @brief loads up needed resources such as textures, sfx, music etc..
        bool LoadResources();

    private:
        static Engine* s_Instance;
        Window* m_Window;
        Shader* m_Shader;
        Shader* m_MainShader;
        
        Camera* m_Camera;
        
        IObject* m_SelectedObject;
        std::vector<IObject*> m_SceneObjects;
        std::vector<PointLight*> m_LightObjects;
        const int MAX_LIGHTS = 8;
        DirectionalLight* m_DirLight;
        bool m_BlinnPhongShading;
        bool m_UseMaterial = true;

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

        /// @brief Deleted copy/move constructors and assignment operators.
        Engine(const Engine &other) = delete;
        Engine &operator=(const Engine &other) = delete;
        Engine(Engine &&other) = delete;
        Engine &operator=(Engine &&other) = delete;
    };


} // namespace isaacGraphicsEngine