#pragma once

#include "Camera/Camera.h"
#include "Graphics/Cube.h"
#include "Graphics/Renderer.h"
#include "Graphics/Shader.h"
#include "Utility/config.h"

namespace isaacObjectLoader
{
class Engine
{
public:
    static Engine* get()
    {
        if (s_Instance == nullptr)
        {
            s_Instance = new Engine();
        }
        return s_Instance;
    }

    void RunEngine();

    // @brief initializes the engine's dependencies, resources and engine objects
    bool Init(const char* title, int width, int height, bool fullscreen = false);

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

    float& getMouseSensitivity() { return m_MouseSensitivity; }
    bool&  getFirstMouse() { return firstMouse; }
    float& getLastMouseX() { return lastX; }
    float& getLastMouseY() { return lastY; }
    float& getMouseYaw() { return yaw; }
    float& getMousePitch() { return pitch; }

    Camera* GetMainCamera() { return m_Camera; }
    bool    GetDisableInput() { return m_DisableInput; }

private:
    Engine();

    void processInput(GLFWwindow* window);

    void imguiInit();
    void imguiNewFrame();
    void imguiRender();
    void imguiCenterItem(float item_width);
    void imguiSetCustomColorStyle();

private:
    static Engine* s_Instance;
    Shader*        m_Shader;
    Shader*        m_lightingShader;
    Shader*        m_lightCubeShader;
    Cube*          m_Cube;
    GLFWwindow*    m_Window;
    GLFWmonitor*   m_PrimaryMonitor;
    Camera*        m_Camera;
    Renderer       m_Renderer;

    bool m_DisableInput;

    float m_DeltaTime = 0.0f;  // time between current frame and last frame
    float m_LastFrame = 0.0f;

    glm::vec3 cubeColor;
    glm::vec3 lightColor;

    glm::vec3 modelPosA;
    glm::vec3 lightPos;
    glm::vec3 m_BackgroundColor;
    glm::mat4 modelTranslation;

    float specularIntensity;

    const char* glsl_version = "#version 400";

    // Mouse
    bool  firstMouse = true;
    float yaw
        = -90.0f;  // yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction
                   // vector pointing to the right so we initially rotate a bit to the left.
    float pitch = 0.0f;
    float lastX = 800.0f / 2.0;
    float lastY = 600.0 / 2.0;
    float fov   = 45.0f;

    float                  m_MouseSensitivity;
    static constexpr float defaultMouseSensitivity = 0.1;

    // ImGui stuff
    ImGuiIO*  m_io;
    bool      show_my_window = false;
    glm::vec4 clear_color    = glm::vec4(0.45f, 0.55f, 0.60f, 1.00f);
    bool*     p_open         = nullptr;

    Engine(const Engine& other)            = delete;
    Engine& operator=(const Engine& other) = delete;
    Engine(Engine&& other)                 = delete;
    Engine& operator=(Engine&& other)      = delete;
};

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);


void setImguiDocking(bool* p_open);
void ShowDockingDisabledMessage();
void HelpMarker(const char* desc);


}  // namespace isaacObjectLoader