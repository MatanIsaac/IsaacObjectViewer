#pragma once

#include "Utility/config.h"
#include "Window.h"
#include "Engine/Scene/Camera/Camera.h"
#include "Engine/Graphics/OpenGL/Primitives/Sphere.h"
#include "Engine/Graphics/OpenGL/Primitives/Plane.h"
#include "Engine/Graphics/OpenGL/Primitives/Cube.h"
#include "Engine/Graphics/OpenGL/Primitives/Cylinder.h"
#include "Engine/Graphics/OpenGL/Lighting/Light.h"
#include "Engine/Graphics/OpenGL/Renderer/Renderer.h"
#include "Engine/Graphics/OpenGL/Shaders/Shader.h"


namespace isaacGraphicsEngine
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
        void Run();
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

        bool GetDisableInput() { return m_DisableInput; }
        bool GetKeyPressed() { return m_KeyPressed; }
        void SetDisableInput(bool disable) { m_DisableInput = disable; }
        void SetKeyPressed(bool pressed) { m_KeyPressed = pressed; }
        
        // ImGui stuff
        //-----------------------------------------------------------------------
        bool GetShowMyWindow() { return m_ShowMyWindow; }
        void SetShowMyWindow(bool show) { m_ShowMyWindow = show; }
        //-----------------------------------------------------------------------

    private:
        Engine();

        // ImGui stuff
        //-----------------------------------------------------------------------
        void ImguiInit();
        void ImguiNewFrame();
        void ImguiRender();
        void ImguiCenterItem(float item_width);
        void ImguiSetCustomColorStyle();
        //-----------------------------------------------------------------------

        // Cube stuff
        //-----------------------------------------------------------------------
        inline void AddCube(const glm::vec3& position = {1.0f, 1.0f, 1.0f})
        {
            m_Cubes.push_back(new Cube(position));
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
                delete cube;
            }
            m_Cubes.clear();
        }
        void AddCubes(int addAmount, float minRange = -20.0f, float maxRange = 20.0f);
        size_t GetCubeCount() const { return m_Cubes.size();}
        void PrintSize() { std::cout << "Number of cubes: " << m_Cubes.size() << std::endl; }
     
    private:
        static Engine *s_Instance;
        Window *m_Window;
        Shader *m_Shader;
        Shader *m_lightingShader;
        
        Sphere* m_Sphere;
        Plane* m_Plane;
        std::vector<Cube*> m_Cubes;
        Cylinder *m_Cylinder;
        Light *m_Light;
        
        Camera *m_Camera;
        Renderer m_Renderer;

        bool m_DisableInput;
        bool m_KeyPressed;
        bool m_IsRunning;

        float m_DeltaTime = 0.0f; // time between current frame and last frame
        float m_LastFrame = 0.0f;
        float m_FPS = 0.0f; // Stores the calculated FPS

        glm::vec3 m_BackgroundColor;

        const char *glsl_version = "#version 400";

        // ImGui stuff
        //-----------------------------------------------------------------------
        ImGuiIO *m_IO;
        bool m_ShowMyWindow;
        glm::vec4 m_ClearColor = glm::vec4(0.45f, 0.55f, 0.60f, 1.00f);
        bool *m_Open = nullptr;
        //-----------------------------------------------------------------------

        Engine(const Engine &other) = delete;
        Engine &operator=(const Engine &other) = delete;
        Engine(Engine &&other) = delete;
        Engine &operator=(Engine &&other) = delete;
    };

    void FramebufferSizeCallback(GLFWwindow *window, int width, int height);
    void MouseCallback(GLFWwindow *window, double xposIn, double yposIn);
    void ScrollCallback(GLFWwindow *window, double xoffset, double yoffset);
    void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);

    void SetImguiDocking(bool *p_open);
    void ShowDockingDisabledMessage();
    void HelpMarker(const char *desc);

} // namespace isaacGraphicsEngine