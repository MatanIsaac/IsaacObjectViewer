#pragma once

#include "Utility/config.h"
#include "Window.h"
#include "Engine/Scene/Camera/Camera.h"
#include "Scene/ISceneObject.h"
#include "Graphics/OpenGL/Primitives/Sphere.h"
#include "Graphics/OpenGL/Primitives/Plane.h"
#include "Graphics/OpenGL/Primitives/Cube.h"
#include "Graphics/OpenGL/Primitives/Cylinder.h"
#include "Engine/Graphics/OpenGL/Primitives/Line.h"
#include "Engine/Graphics/OpenGL/Lighting/Light.h"
#include "Engine/Graphics/OpenGL/Renderer/Renderer.h"
#include "Engine/Graphics/OpenGL/Shaders/Shader.h"

namespace isaacObjectLoader
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

        Light* GetLight() { return m_Light; }

        void EnableMouseMode();
        void EnableFreeCameraMode();
        
        // Cube stuff
        //-----------------------------------------------------------------------
        
        Cube* GetCube(size_t index)
        {
            if (index < m_Cubes.size())
                return m_Cubes[index];
            return nullptr;
        }

        inline void AddCube(const glm::vec3& position = {1.0f, 1.0f, 1.0f})
        {
            m_Cubes.push_back(new Cube(position));
            m_SceneObjects.push_back(new Cube(position));
        }
        inline void RemoveCube(size_t index)
        {
            if (index < m_Cubes.size())
            {
                delete m_Cubes[index];
                m_Cubes.erase(m_Cubes.begin() + index);
            }
        }
        inline void ClearCubes()
        {
            for (auto cube : m_Cubes)
            {
                if(cube != nullptr)
                    delete cube;
            }
            m_Cubes.clear();
        }
        std::vector<Cube*>& GetCubes() { return m_Cubes; }
        void AddCubes(int addAmount, float minRange = -20.0f, float maxRange = 20.0f);
        size_t GetCubeCount() const { return m_Cubes.size();}
        void PrintSize() { std::cout << "Number of cubes: " << m_Cubes.size() << std::endl; }
        //-----------------------------------------------------------------------

        // Primitives
        //-----------------------------------------------------------------------
        std::vector<ISceneObject*>& GetSceneObjects() { return m_SceneObjects; }
        ISceneObject* GetSelectedObject() { return m_SelectedObject; }
        void SetSelectedObject(ISceneObject* obj) { m_SelectedObject = obj; }
        inline void ClearSceneObjects()
        {
            for (auto obj : m_SceneObjects)
            {
                if(obj != nullptr)
                    delete obj;
            }
            m_SceneObjects.clear();
        }
        //-----------------------------------------------------------------------
    private:
        Engine();
        
    private:
        static Engine* s_Instance;
        Window* m_Window;
        Shader* m_Shader;
        Shader* m_lightingShader;
        
        std::vector<Cube*> m_Cubes;
        Light* m_Light;
        Camera* m_Camera;
        
        ISceneObject* m_SelectedObject;
        std::vector<ISceneObject*> m_SceneObjects;

        Renderer m_Renderer;

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