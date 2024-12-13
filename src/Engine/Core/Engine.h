#pragma once

#include "Utility/config.h"
#include "Window.h"
#include "Engine/Scene/Camera/Camera.h"
#include "Engine/Graphics/OpenGL/Primitives/Cube.h"
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

    private:
        static Engine *s_Instance;
        Window *m_Window;
        Shader *m_Shader;
        Shader *m_lightingShader;
        Shader *m_lightCubeShader;
        Cube *m_Cube;
        Light *m_Light;
        
        Camera *m_Camera;
        Renderer m_Renderer;

        bool m_DisableInput;
        bool m_KeyPressed;
        bool m_IsRunning;

        float m_DeltaTime = 0.0f; // time between current frame and last frame
        float m_LastFrame = 0.0f;
        
        glm::vec3 m_BackgroundColor;
        glm::mat4 m_ModelTranslation;

        const char *glsl_version = "#version 400";

        // ImGui stuff
        //-----------------------------------------------------------------------
        ImGuiIO *m_IO;
        bool m_ShowMyWindow = false;
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