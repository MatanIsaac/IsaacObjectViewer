#pragma once

#include "Engine/Scene/Camera/Camera.h"
#include "Engine/Scene/Primitives/Cube.h"
#include "Engine/Graphics/OpenGL/Renderer/Renderer.h"
#include "Engine/Graphics/OpenGL/Shaders/Shader.h"
#include "Utility/config.h"


namespace isaacObjectLoader
{
    class Engine
    {
    public:
        static Engine *get()
        {
            if (s_Instance == nullptr)
            {
                s_Instance = new Engine();
            }
            return s_Instance;
        }

        // @brief Starts the engine.
        void RunEngine();

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

        float &GetMouseSensitivity() { return m_MouseSensitivity; }
        bool &GetFirstMouse() { return m_FirstMouse; }
        float &GetLastMouseX() { return m_LastX; }
        float &GetLastMouseY() { return m_LastY; }
        float &GetMouseYaw() { return m_MouseYaw; }
        float &GetMousePitch() { return m_Pitch; }

        Camera *GetMainCamera() { return m_Camera; }
        bool GetDisableInput() { return m_DisableInput; }
        bool GetKeyPressed() { return m_KeyPressed; }
        bool GetShowMyWindow() { return m_ShowMyWindow; }

        void SetDisableInput(bool disable) { m_DisableInput = disable; }
        void SetKeyPressed(bool pressed) { m_KeyPressed = pressed; }
        void SetShowMyWindow(bool show) { m_ShowMyWindow = show; }

    private:
        Engine();

        void ImguiInit();
        void ImguiNewFrame();
        void ImguiRender();
        void ImguiCenterItem(float item_width);
        void ImguiSetCustomColorStyle();

    private:
        static Engine *s_Instance;
        Shader *m_Shader;
        Shader *m_lightingShader;
        Shader *m_lightCubeShader;
        Cube *m_Cube;
        GLFWwindow *m_Window;
        GLFWmonitor *m_PrimaryMonitor;
        Camera *m_Camera;
        Renderer m_Renderer;

        bool m_DisableInput;
        bool m_KeyPressed;

        float m_DeltaTime = 0.0f; // time between current frame and last frame
        float m_LastFrame = 0.0f;

        glm::vec3 m_CubeColor;
        glm::vec3 m_LightColor;

        glm::vec3 m_ModelPosA;
        glm::vec3 m_LightPos;
        glm::vec3 m_BackgroundColor;
        glm::mat4 m_ModelTranslation;

        float m_SpecularIntensity;

        const char *glsl_version = "#version 400";

        // Mouse
        bool m_FirstMouse = true;
        float m_MouseYaw = -90.0f; // yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction
                                   // vector pointing to the right so we initially rotate a bit to the left.
        float m_Pitch = 0.0f;
        float m_LastX = 800.0f / 2.0;
        float m_LastY = 600.0 / 2.0;
        float m_FOV = 45.0f;

        float m_MouseSensitivity;
        static constexpr float m_DefaultMouseSensitivity = 0.1;

        // ImGui stuff
        ImGuiIO *m_IO;
        bool m_ShowMyWindow = false;
        glm::vec4 m_ClearColor = glm::vec4(0.45f, 0.55f, 0.60f, 1.00f);
        bool *m_Open = nullptr;

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

} // namespace isaacObjectLoader