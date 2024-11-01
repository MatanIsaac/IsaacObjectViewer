#include "Engine.h"
#include <cassert>
#include <chrono>

#include "Utility/ColorMacros.h"

namespace isaacObjectLoader
{
Engine* Engine::s_Instance = nullptr;

Engine::Engine()
: m_Shader(nullptr)
, m_lightingShader(nullptr)
, m_lightCubeShader(nullptr)
, m_Cube(nullptr)
, m_Window(nullptr)
, m_PrimaryMonitor(nullptr)
, m_Camera(nullptr)
, m_io(nullptr)
{
}

void Engine::RunEngine()
{
    assert(Init("Isaac's Object Loader", 1280, 720, true));

    while (!glfwWindowShouldClose(m_Window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        m_DeltaTime        = currentFrame - m_LastFrame;
        m_LastFrame        = currentFrame;

        // input
        ProcessInput();
        Update(m_DeltaTime);
        Render();

        glfwSwapBuffers(m_Window);
        glfwPollEvents();
    }
    Clean();
}

// @brief initializes the engine's dependencies, resources and objects
bool Engine::Init(const char* title, int width, int height, bool fullscreen)
{
    // Initialize GLFW
    //----------------------------------------------------------------------------------------
    if (!glfwInit())
    {
        std::cout << "Failed to initialize GLFW" << '\n';
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    if (fullscreen)
    {
        // Get the primary monitor
        m_PrimaryMonitor = glfwGetPrimaryMonitor();
        // Get the video mode of the primary monitor
        const GLFWvidmode* mode = glfwGetVideoMode(m_PrimaryMonitor);

        m_Window = glfwCreateWindow(mode->width, mode->height, title, nullptr, nullptr);
        if (!m_Window)
        {
            std::cout << "Failed to create a glfw window! - " << '\n';
            return false;
        }
    }
    else
    {
        m_Window = glfwCreateWindow(width, height, title, nullptr, nullptr);
        if (!m_Window)
        {
            std::cout << "Failed to create a glfw window! - " << '\n';
            return false;
        }
    }

    glfwMakeContextCurrent(m_Window);
    glfwSwapInterval(1);  // Enable vsync

    glfwSetFramebufferSizeCallback(m_Window, framebuffer_size_callback);
    glfwSetCursorPosCallback(m_Window, mouse_callback);
    glfwSetScrollCallback(m_Window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    //----------------------------------------------------------------------------------------

    // Initialize glad
    //----------------------------------------------------------------------------------------
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
    {
        std::cout << "Failed to load OpenGL!\n";
        glfwTerminate();
        return false;
    }
    //----------------------------------------------------------------------------------------

    glEnable(GL_DEPTH_TEST);
    // Enable blending to allow for transparent textures and effects.
    glEnable(GL_BLEND);
    // Set the blending function to use source alpha and one minus source alpha blending mode.
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    imguiInit();

    m_Cube = new Cube();
    // m_Shader = new Shader("X:\\Programming\\CPP\\ObjectLoader\\src\\shaders\\vertex.vs",
    // "X:\\Programming\\CPP\\ObjectLoader\\src\\shaders\\fragment.fs");
    m_lightingShader  = new Shader("src\\shaders\\colors.vs", "src\\shaders\\colors.fs");
    m_lightCubeShader = new Shader("src\\shaders\\light_cube.vs", "src\\shaders\\light_cube.fs");

    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once
    // m_Shader->Bind(); // must bind the shader before setting uniforms
    // m_Shader->setInt("texture1", 0);

    cubeColor  = { 1.0f, 0.5f, 0.31f };
    lightColor = { 1.0f, 1.0f, 1.0f };
    m_lightingShader->Bind();

    // Set the position of the quad in the scene using a model matrix.
    modelPosA = { 0.0f, 0.0f, 0.0f };  // Define the translation vector for the quad's position.
    lightPos  = { 1.2f, 1.0f, 2.0f };
    modelTranslation = glm::mat4(1.0f);
    modelTranslation = glm::translate(modelTranslation, modelPosA);

    m_BackgroundColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

    m_Camera = new Camera(glm::vec3(0.0f, 0.0f, 3.0f));

    m_MouseSensitivity = defaultMouseSensitivity;  // change this value to your liking

    m_DisableInput = false;

    specularIntensity = 0.5;

    return true;
}

// @brief processes player input
void Engine::ProcessInput() { processInput(m_Window); }

// @brief updates all of the engine dependencies, resources and objects.
void Engine::Update(float dt) {}  // std::cout << "DeltaTime: " << 1 / dt << '\n';

// @brief renders all of the engine textures, sounds and objects.
void Engine::Render()
{
    // Rendering
    int display_w, display_h;
    glfwGetFramebufferSize(m_Window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);

    imguiNewFrame();
    imguiRender();

    // render
    glClearColor(m_BackgroundColor.x, m_BackgroundColor.y, m_BackgroundColor.z, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // MODEL
    glm::mat4 model = glm::mat4(1.0f);
    model           = glm::translate(model, modelPosA);
    float angle     = 0.0f;
    model           = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.0f, 0.0f));
    // rotate the model
    // model = glm::rotate(
    //     model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));

    // VIEW
    glm::mat4 view = m_Camera->GetViewMatrix();

    // PROJECTION
    glm::mat4 projection = glm::perspective(glm::radians(m_Camera->GetCameraZoom()),
                                            (float) display_w / (float) display_h,
                                            0.1f,
                                            100.0f);

    // 'model' matrix.
    // m_Shader->setMat4("model", model); // Upload the translation matrix to the shader as the
    // m_Shader->setMat4("view",
    //                  view); // Upload the translation matrix to the shader as the 'model' matrix.
    // m_Shader->setMat4(
    //    "projection",
    //    projection); // Upload the translation matrix to the shader as the 'model' matrix.

    // 'model' matrix.
    m_lightingShader->Bind();
    m_lightingShader->setMat4("model", model);
    m_lightingShader->setMat4("view", view);
    m_lightingShader->setMat4("projection", projection);
    m_lightingShader->setVec3("objectColor", cubeColor);
    m_lightingShader->setVec3("lightColor", lightColor);
    m_lightingShader->setVec3("lightPos", lightPos);
    m_lightingShader->setVec3("viewPos", m_Camera->GetCameraPosition());
    m_lightingShader->setFloat("specularIntensity", specularIntensity);

    // Rotating Cube
    m_Renderer.Render(m_Cube->GetCubeVA(), m_Cube->GetVertexCount(), *m_lightingShader);

    // render light cube
    m_lightCubeShader->Bind();
    m_lightCubeShader->setMat4("projection", projection);
    m_lightCubeShader->setMat4("view", view);
    model = glm::mat4(1.0f);
    model = glm::translate(model, lightPos);
    model = glm::scale(model, glm::vec3(0.2f));  // a smaller cube
    m_lightCubeShader->setMat4("model", model);

    m_Renderer.Render(m_Cube->GetCubeVA(), m_Cube->GetVertexCount(), *m_lightCubeShader);

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

// @brief cleans all of the engine resources.
void Engine::Clean()
{
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    delete m_io;
    delete m_Cube;
    delete m_Shader;
    glfwDestroyWindow(m_Window);
    glfwTerminate();
}

// @brief loads up needed resources such as textures, sfx, music etc..
bool Engine::LoadResources() { return false; }

void Engine::processInput(GLFWwindow* window)
{
    // In your main loop or appropriate input handling section
    if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
    {
        // glfwSetWindowShouldClose(window, true); // Uncomment if you want to close the window on
        // ESC
        if (!m_DisableInput)
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            m_DisableInput = true;
            show_my_window = !show_my_window;
        }
        else
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            m_DisableInput = false;
            show_my_window = !show_my_window;
        }
    }

    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
    {
        show_my_window = !show_my_window;
    }

    if (!m_DisableInput)
    {
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            m_Camera->ProcessKeyboard(FORWARD, m_DeltaTime);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            m_Camera->ProcessKeyboard(BACKWARD, m_DeltaTime);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            m_Camera->ProcessKeyboard(LEFT, m_DeltaTime);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            m_Camera->ProcessKeyboard(RIGHT, m_DeltaTime);
    }
}

void Engine::imguiInit()
{
    // Imgui-Docking Initialization
    //-------------------------------------------------------------------------------------------------
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    m_io = &ImGui::GetIO();
    (void) m_io;
    m_io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
    m_io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;      // Enable Docking

    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(m_Window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    //-------------------------------------------------------------------------------------------------
}
void Engine::imguiNewFrame()
{
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    setImguiDocking(p_open);
}

void Engine::imguiRender()
{
    imguiSetCustomColorStyle();

    // imgui render
    if (show_my_window)
    {
        ImGui::Begin("Scene Settings", &show_my_window);

        if (ImGui::CollapsingHeader("Environment Settings"))
        {
            ImGui::ColorEdit3("Background Color", (float*) &m_BackgroundColor);

            if (ImGui::Button("Reset Background Color"))
            {
                m_BackgroundColor = { 0.0f, 0.0f, 0.0f };
                ImGui::SetTooltip("Reset background color to default.");
            }
        }

        if (ImGui::CollapsingHeader("Camera Settings"))
        {
            ImGui::SliderFloat("Camera Speed", &m_Camera->GetCameraSpeed(), 0.1f, 1.0f, "%.1f");
            if (ImGui::Button("Reset Speed"))
            {
                m_Camera->ResetCameraSpeed();
                ImGui::SetTooltip("Reset camera speed to initial value.");
            }
            ImGui::SameLine();
            if (ImGui::Button("Reset Position"))
            {
                m_Camera->SetCameraPosition(glm::vec3(0.0f, 0.0f, 3.0f));
                ImGui::SetTooltip("Reset camera to default position.");
            }
        }

        if (ImGui::CollapsingHeader("Mouse Settings"))
        {
            ImGui::SliderFloat(
                "Mouse Sensitivity", &m_Camera->GetMouseSensitivity(), 0.01f, 0.1f, "%.2f");
            if (ImGui::Button("Reset Mouse Sensitivity"))
            {
                m_Camera->ResetMouseSensitivity();
                ImGui::SetTooltip("Reset mouse sensitivity.");
            }
        }

        if (ImGui::CollapsingHeader("Cube Settings"))
        {
            ImGui::SliderFloat3("Rotating Cube Position", (float*) &modelPosA, -10.f, 10.f);
            ImGui::ColorEdit3("Cube Color", (float*) &cubeColor);
        }

        if (ImGui::CollapsingHeader("Light Cube Settings"))
        {
            ImGui::SliderFloat3("Light Cube Position", (float*) &lightPos, -10.f, 10.f);
            // cubeColor
            ImGui::ColorEdit3("Light Cube Color", (float*) &lightColor);
            ImGui::InputFloat("Specular Intensity", &specularIntensity, 0.0f, 1.0f);
        }

        ImGui::End();
    }

    ImGui::End();
    ImGui::Render();
}

void Engine::imguiCenterItem(float itemWidth)
{
    float windowWidth = ImGui::GetContentRegionAvail().x;
    ImGui::SetCursorPosX((windowWidth - itemWidth)
                         * 0.5f);  // Set the cursor position to the middle
}

void Engine::imguiSetCustomColorStyle()
{
    // Set style
    ImGuiStyle* style = &ImGui::GetStyle();

    // Assigning colors to various ImGui components using your defined colors
    style->Colors[ImGuiCol_Text]         = IMGUI_COLOR_WHITE;
    style->Colors[ImGuiCol_TextDisabled] = IMGUI_COLOR_GREY;
    style->Colors[ImGuiCol_WindowBg]     = IMGUI_COLOR_DARKGREY;   // Background of window
    style->Colors[ImGuiCol_ChildBg]      = IMGUI_COLOR_LIGHTGREY;  // Background of child windows
    style->Colors[ImGuiCol_PopupBg]      = IMGUI_COLOR_LIGHTGREY;  // Background of popups
    style->Colors[ImGuiCol_Border]       = IMGUI_COLOR_DARKBLUE;
    style->Colors[ImGuiCol_BorderShadow] = IMGUI_COLOR_BLACK;
    style->Colors[ImGuiCol_FrameBg]
        = IMGUI_COLOR_BLUEGREY;  // Background of checkbox, radio button, plot, slider, text input
    style->Colors[ImGuiCol_FrameBgHovered]       = IMGUI_COLOR_BLUE;
    style->Colors[ImGuiCol_FrameBgActive]        = IMGUI_COLOR_DARKBLUE;
    style->Colors[ImGuiCol_TitleBg]              = IMGUI_COLOR_BLUE;
    style->Colors[ImGuiCol_TitleBgActive]        = IMGUI_COLOR_DARKBLUE;
    style->Colors[ImGuiCol_TitleBgCollapsed]     = IMGUI_COLOR_GREY;
    style->Colors[ImGuiCol_MenuBarBg]            = IMGUI_COLOR_BLUEGREY;
    style->Colors[ImGuiCol_ScrollbarBg]          = IMGUI_COLOR_BLUEGREY;
    style->Colors[ImGuiCol_ScrollbarGrab]        = IMGUI_COLOR_GREY;
    style->Colors[ImGuiCol_ScrollbarGrabHovered] = IMGUI_COLOR_LIGHTGREY;
    style->Colors[ImGuiCol_ScrollbarGrabActive]  = IMGUI_COLOR_DARKGREY;
    style->Colors[ImGuiCol_CheckMark]            = IMGUI_COLOR_BLUEGREY;
    style->Colors[ImGuiCol_SliderGrab]           = IMGUI_COLOR_BLUEGREY;
    style->Colors[ImGuiCol_SliderGrabActive]     = IMGUI_COLOR_DARKBLUE;
    style->Colors[ImGuiCol_Button]               = IMGUI_COLOR_BLUEGREY;  // Normal button
    style->Colors[ImGuiCol_ButtonHovered]        = IMGUI_COLOR_BLUEGREY;  // Button on hover
    style->Colors[ImGuiCol_ButtonActive]         = IMGUI_COLOR_DARKBLUE;  // Button when clicked
    style->Colors[ImGuiCol_Header]
        = IMGUI_COLOR_DARKGREY;  // Header background (e.g., CollapsingHeader)
    style->Colors[ImGuiCol_HeaderHovered] = IMGUI_COLOR_BLUEGREY;  // Header background when hovered
    style->Colors[ImGuiCol_HeaderActive]  = IMGUI_COLOR_DARKBLUE;  // Header background when active
    style->Colors[ImGuiCol_Separator]     = IMGUI_COLOR_DARKGREY;
    style->Colors[ImGuiCol_SeparatorHovered]     = IMGUI_COLOR_GREY;
    style->Colors[ImGuiCol_SeparatorActive]      = IMGUI_COLOR_WHITE;
    style->Colors[ImGuiCol_ResizeGrip]           = IMGUI_COLOR_LIGHTGREY;
    style->Colors[ImGuiCol_ResizeGripHovered]    = IMGUI_COLOR_GREY;
    style->Colors[ImGuiCol_ResizeGripActive]     = IMGUI_COLOR_DARKGREY;
    style->Colors[ImGuiCol_Tab]                  = IMGUI_COLOR_LIGHTGREY;
    style->Colors[ImGuiCol_TabHovered]           = IMGUI_COLOR_GREY;
    style->Colors[ImGuiCol_TabActive]            = IMGUI_COLOR_DARKGREY;
    style->Colors[ImGuiCol_TabUnfocused]         = IMGUI_COLOR_LIGHTGREY;
    style->Colors[ImGuiCol_TabUnfocusedActive]   = IMGUI_COLOR_GREY;
    style->Colors[ImGuiCol_PlotLines]            = IMGUI_COLOR_YELLOW;
    style->Colors[ImGuiCol_PlotLinesHovered]     = IMGUI_COLOR_ORANGE;
    style->Colors[ImGuiCol_PlotHistogram]        = IMGUI_COLOR_RED;
    style->Colors[ImGuiCol_PlotHistogramHovered] = IMGUI_COLOR_DARKRED;
    style->Colors[ImGuiCol_TextSelectedBg]       = IMGUI_COLOR_LIGHTGREY;
    style->Colors[ImGuiCol_DragDropTarget]       = IMGUI_COLOR_GREEN;
    style->Colors[ImGuiCol_NavHighlight]         = IMGUI_COLOR_BLUE;  // Navigation highlight color
    style->Colors[ImGuiCol_NavWindowingHighlight]
        = IMGUI_COLOR_LIGHTBLUE;  // Navigation windowing highlight
    style->Colors[ImGuiCol_NavWindowingDimBg]
        = IMGUI_COLOR_PURPLE;  // Navigation windowing dim background, covers entire screen
    style->Colors[ImGuiCol_ModalWindowDimBg]
        = IMGUI_COLOR_DARKPURPLE;  // Modal window dim background, covers entire screen
}

void framebuffer_size_callback([[maybe_unused]] GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void mouse_callback([[maybe_unused]] GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    auto  engine      = Engine::get();
    auto& first_mouse = engine->getFirstMouse();
    auto& last_x      = engine->getLastMouseX();
    auto& last_y      = engine->getLastMouseY();

    if (first_mouse)
    {
        last_x      = xpos;
        last_y      = ypos;
        first_mouse = false;
    }

    float xoffset = xpos - last_x;
    float yoffset = last_y - ypos;  // reversed since y-coordinates go from bottom to top

    last_x = xpos;
    last_y = ypos;

    if (!engine->GetDisableInput())
    {
        engine->GetMainCamera()->ProcessMouseMovement(xoffset, yoffset);
    }
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback([[maybe_unused]] GLFWwindow* window,
                     [[maybe_unused]] double      xoffset,
                     double                       yoffset)
{
    Engine::get()->GetMainCamera()->ProcessMouseScroll(static_cast<float>(yoffset));
}

// Helper to display a little (?) mark which shows a tooltip when hovered.
// In your own code you may want to display an actual icon if you are using a merged icon fonts (see
// docs/FONTS.md)
void HelpMarker(const char* desc)
{
    ImGui::TextDisabled("(?)");
    if (ImGui::BeginItemTooltip())
    {
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

void ShowDockingDisabledMessage()
{
    ImGuiIO& io = ImGui::GetIO();
    ImGui::Text("ERROR: Docking is not enabled! See Demo > Configuration.");
    ImGui::Text("Set io.ConfigFlags |= ImGuiConfigFlags_DockingEnable in your code, or ");
    ImGui::SameLine(0.0f, 0.0f);
    if (ImGui::SmallButton("click here"))
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
}

void setImguiDocking(bool* p_open)
{
    // READ THIS !!!
    // TL;DR; this demo is more complicated than what most users you would normally use.
    // If we remove all options we are showcasing, this demo would become:
    //     void ShowExampleAppDockSpace()
    //     {
    //         ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
    //     }
    // In most cases you should be able to just call DockSpaceOverViewport() and ignore all the code
    // below! In this specific demo, we are not using DockSpaceOverViewport() because:
    // - (1) we allow the host window to be floating/moveable instead of filling the viewport (when
    // opt_fullscreen == false)
    // - (2) we allow the host window to have padding (when opt_padding == true)
    // - (3) we expose many flags and need a way to have them visible.
    // - (4) we have a local menu bar in the host window (vs. you could use BeginMainMenuBar() +
    // DockSpaceOverViewport()
    //      in your code, but we don't here because we allow the window to be floating)

    static bool               opt_fullscreen = true;
    static bool               opt_padding    = false;
    static ImGuiDockNodeFlags dockspace_flags
        = ImGuiDockNodeFlags_None | ImGuiDockNodeFlags_PassthruCentralNode;

    // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
    // because it would be confusing to have two docking targets within each others.
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    if (opt_fullscreen)
    {
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse
                        | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    }
    else
    {
        dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
    }

    // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
    // and handle the pass-thru hole, so we ask Begin() to not render a background.
    if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
        window_flags |= ImGuiWindowFlags_NoBackground;

    // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
    // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
    // all active windows docked into it will lose their parent and become undocked.
    // We cannot preserve the docking relationship between an active window and an inactive docking,
    // otherwise any change of dockspace/settings would lead to windows being stuck in limbo and
    // never being visible.
    if (!opt_padding)
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("DockSpace Demo", p_open, window_flags);
    if (!opt_padding)
        ImGui::PopStyleVar();

    if (opt_fullscreen)
        ImGui::PopStyleVar(2);

    // Submit the DockSpace
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
    {
        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
    }
    else
    {
        ShowDockingDisabledMessage();
    }

    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("Options"))
        {
            // Disabling fullscreen would allow the window to be moved to the front of other
            // windows, which we can't undo at the moment without finer window depth/z control.
            ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen);
            ImGui::MenuItem("Padding", NULL, &opt_padding);
            ImGui::Separator();

            if (ImGui::MenuItem("Flag: NoDockingOverCentralNode",
                                "",
                                (dockspace_flags & ImGuiDockNodeFlags_NoDockingOverCentralNode)
                                    != 0))
            {
                dockspace_flags ^= ImGuiDockNodeFlags_NoDockingOverCentralNode;
            }
            if (ImGui::MenuItem("Flag: NoDockingSplit",
                                "",
                                (dockspace_flags & ImGuiDockNodeFlags_NoDockingSplit) != 0))
            {
                dockspace_flags ^= ImGuiDockNodeFlags_NoDockingSplit;
            }
            if (ImGui::MenuItem("Flag: NoUndocking",
                                "",
                                (dockspace_flags & ImGuiDockNodeFlags_NoUndocking) != 0))
            {
                dockspace_flags ^= ImGuiDockNodeFlags_NoUndocking;
            }
            if (ImGui::MenuItem(
                    "Flag: NoResize", "", (dockspace_flags & ImGuiDockNodeFlags_NoResize) != 0))
            {
                dockspace_flags ^= ImGuiDockNodeFlags_NoResize;
            }
            if (ImGui::MenuItem("Flag: AutoHideTabBar",
                                "",
                                (dockspace_flags & ImGuiDockNodeFlags_AutoHideTabBar) != 0))
            {
                dockspace_flags ^= ImGuiDockNodeFlags_AutoHideTabBar;
            }
            if (ImGui::MenuItem("Flag: PassthruCentralNode",
                                "",
                                (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode) != 0,
                                opt_fullscreen))
            {
                dockspace_flags ^= ImGuiDockNodeFlags_PassthruCentralNode;
            }
            ImGui::Separator();

            if (ImGui::MenuItem("Close", NULL, false, p_open != NULL))
                *p_open = false;
            ImGui::EndMenu();
        }
        HelpMarker(
            "When docking is enabled, you can ALWAYS dock MOST window into another! Try it now!"
            "\n"
            "- Drag from window title bar or their tab to dock/undock."
            "\n"
            "- Drag from window menu button (upper-left button) to undock an entire node (all windows)."
            "\n"
            "- Hold SHIFT to disable docking (if io.ConfigDockingWithShift == false, default)"
            "\n"
            "- Hold SHIFT to enable docking (if io.ConfigDockingWithShift == true)"
            "\n"
            "This demo app has nothing to do with enabling docking!"
            "\n\n"
            "This demo app only demonstrate the use of ImGui::DockSpace() which allows you to manually create a "
            "docking node _within_ another window."
            "\n\n"
            "Read comments in ShowExampleAppDockSpace() for more details.");

        ImGui::EndMenuBar();
    }
}

}  // namespace isaacObjectLoader
