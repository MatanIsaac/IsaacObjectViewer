#include "Engine.h"
#include <algorithm>
#include "Utility/ColorMacros.h"
#include "Utility/Log.hpp"
#include "Mouse.h"

namespace isaacGraphicsEngine
{
    Engine *Engine::s_Instance = nullptr;

    Engine::Engine()
        : m_Shader(nullptr),
          m_lightingShader(nullptr),
          m_Cubes({}), //m_Cube(nullptr), 
          m_Window(nullptr),
          m_Camera(nullptr),
          m_DisableInput(false),
          m_KeyPressed(false),
          m_IsRunning(false),
          m_IO(nullptr)
    {}

    void Engine::Run()
    {
        assert(Init("Isaac's Graphics Engine", SCREEN_WIDTH, SCREEN_HEIGHT, true));

        while (m_IsRunning)
        {
            // per-frame time logic
            // --------------------
            float currentFrame = static_cast<float>(glfwGetTime());
            m_DeltaTime = currentFrame - m_LastFrame;
            m_LastFrame = currentFrame;

            // Calculate FPS
            if (m_DeltaTime > 0.0f)
            {
                m_FPS = 1.0f / m_DeltaTime;
            }

            // input
            ProcessInput();
            Update(m_DeltaTime);
            Render();

            glfwSwapBuffers(m_Window->GetGLFWwindow());
            glfwPollEvents();
        }
        Clean();
    }

    // @brief initializes the engine's dependencies, resources and objects
    bool Engine::Init(const char *title, int width, int height, bool fullscreen)
    {
        Log::Init();

        LOG_INFO("Welcome to Isaac's Graphics Engine!");
        m_Window = new Window(title,width,height,false);

        glfwSetFramebufferSizeCallback(m_Window->GetGLFWwindow(), FramebufferSizeCallback);
        glfwSetCursorPosCallback(m_Window->GetGLFWwindow(), MouseCallback);
        glfwSetScrollCallback(m_Window->GetGLFWwindow(), ScrollCallback);
        glfwSetKeyCallback(m_Window->GetGLFWwindow(), KeyCallback);

        // tell GLFW to capture our mouse
        glfwSetInputMode(m_Window->GetGLFWwindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);

        // Initialize glad
        //----------------------------------------------------------------------------------------
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            LOG_ERROR("Failed to load OpenGL!");
            glfwTerminate();
            return false;
        }
        //----------------------------------------------------------------------------------------

        glEnable(GL_DEPTH_TEST); // Enable depth testing
	
        ImguiInit();
        const glm::vec3 plane_pos = {5.f,1.f,1.f};
        m_Plane     = new Plane(plane_pos);
        m_Cylinder  = new Cylinder();
        m_Light     = new Light({1.2f, 1.0f, 2.0f},{1.0f, 1.0f, 1.0f});
        
        std::string projectRoot = GetProjectRoot();

        auto colors_vs  = projectRoot.append("\\src\\Resources\\Shaders\\colors.vs");
        projectRoot     = GetProjectRoot();
        auto colors_fs  = projectRoot.append("\\src\\Resources\\Shaders\\colors.fs");

        m_lightingShader = new Shader(colors_vs.c_str(), colors_fs.c_str());
        m_lightingShader->Bind();
                                        
        m_BackgroundColor = glm::vec4(0.75f, 0.75f, 0.75f, 1.0f);

        m_Camera = new Camera(glm::vec3(0.0f, 2.0f, 5.0f));

        m_DisableInput = true;
        m_ShowMyWindow = true;
        
        m_IsRunning = true;

        return true;
    }

    // @brief processes player input
    void Engine::ProcessInput()
    {
        if (glfwGetKey(m_Window->GetGLFWwindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS) 
        {
            glfwSetInputMode(m_Window->GetGLFWwindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            SetDisableInput(true);
            SetShowMyWindow(true);
        }

        if (!m_DisableInput)
        {
            if (glfwGetKey(m_Window->GetGLFWwindow(), GLFW_KEY_W) == GLFW_PRESS)
                m_Camera->ProcessKeyboard(FORWARD, m_DeltaTime);
            if (glfwGetKey(m_Window->GetGLFWwindow(), GLFW_KEY_S) == GLFW_PRESS)
                m_Camera->ProcessKeyboard(BACKWARD, m_DeltaTime);
            if (glfwGetKey(m_Window->GetGLFWwindow(), GLFW_KEY_A) == GLFW_PRESS)
                m_Camera->ProcessKeyboard(LEFT, m_DeltaTime);
            if (glfwGetKey(m_Window->GetGLFWwindow(), GLFW_KEY_D) == GLFW_PRESS)
                m_Camera->ProcessKeyboard(RIGHT, m_DeltaTime);

        }
    }

    // @brief updates all of the engine dependencies, resources and objects.
    void Engine::Update(float dt) 
    {
        /*
            TODO: 
            if mouse is in scene view box, and not in play mode, 
            and right mouse button is down, allow mouse look around input.
        */
    }

    // @brief renders all of the engine textures, sounds and objects.
    void Engine::Render()
    {
        // Rendering
        int display_w, display_h;
        glfwGetFramebufferSize(m_Window->GetGLFWwindow(), &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);

        ImguiNewFrame();
        ImguiRender();

        // render
        glClearColor(m_BackgroundColor.x, m_BackgroundColor.y, m_BackgroundColor.z, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        m_lightingShader->Bind();
        m_lightingShader->setVec3("viewPos", m_Camera->GetPosition());
                
        glm::mat4 view = m_Camera->GetViewMatrix(); // VIEW
        glm::mat4 projection = glm::perspective(glm::radians(m_Camera->GetZoom()), // PROJECTION
                                                (float)display_w / (float)display_h,
                                                0.1f,
                                                100.0f);
                                                
        // Render Primitives
        //------------------------------------------------------------------------------------
        m_Plane->Render(m_Renderer, *m_lightingShader, view, projection);
        m_Cylinder->Render(m_Renderer, *m_lightingShader, view, projection);
        
        for (auto& cube : m_Cubes)
        {
            cube->Render(m_Renderer,*m_lightingShader, view, projection);
        }
        
        //------------------------------------------------------------------------------------

        // render light
        m_Light->Render(m_Renderer,*m_lightingShader, view, projection);
        
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    // @brief cleans all of the engine resources.
    void Engine::Clean()
    {
        // Cleanup
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        ClearCubes();
        delete m_Plane;
        delete m_Cylinder;
        delete m_IO;
        delete m_Shader;
        glfwDestroyWindow(m_Window->GetGLFWwindow());
        glfwTerminate();
    }

    // @brief loads up needed resources such as textures, sfx, music etc..
    bool Engine::LoadResources() { return false; }

    // -----------------------------------------------------------------------------------------------------
    // Imgui Stuff
    // -----------------------------------------------------------------------------------------------------

    void Engine::ImguiInit()
    {
        // Imgui-Docking Initialization
        //-------------------------------------------------------------------------------------------------
        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        m_IO = &ImGui::GetIO();
        (void)m_IO;
        m_IO->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
        m_IO->ConfigFlags |= ImGuiConfigFlags_DockingEnable;     // Enable Docking

        ImGui::StyleColorsDark();

        // Setup Platform/Renderer backends
        ImGui_ImplGlfw_InitForOpenGL(m_Window->GetGLFWwindow(), true);
        ImGui_ImplOpenGL3_Init(glsl_version);

        //-------------------------------------------------------------------------------------------------
    }
    void Engine::ImguiNewFrame()
    {
        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        SetImguiDocking(m_Open);
    }

    void Engine::ImguiRender()
    {
        //ImguiSetCustomColorStyle();
        ImGuiIO& io = ImGui::GetIO();
        float display_w = io.DisplaySize.x;
        float display_h = io.DisplaySize.y;
    
        // ===================================================
        // Top Control Panel 
        // ===================================================
        {
            int button_num = 3;
            glm::vec2 button_size = {40,20}; // in px
            int spacing = 20;
            // Fixed height for the control panel (e.g., 50 pixels)
            float controlPanelHeight = 20.0f;
            ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
            ImGui::SetNextWindowSize(ImVec2(display_w - 300.f, controlPanelHeight), ImGuiCond_Always);
    
            // No docking, collapse, or moving; and hide title bar for a clean look.
            ImGuiWindowFlags topPanelFlags = ImGuiWindowFlags_NoDocking |
                                             ImGuiWindowFlags_NoCollapse |
                                             ImGuiWindowFlags_NoMove |
                                             ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoDecoration;

            ImGui::Begin("Engine Controls", nullptr, topPanelFlags);
    
            // Center the buttons horizontally:
            float totalButtonWidth = button_size.x * button_num + spacing; // Three buttons at 80 px each plus spacing
            float cursorX = (display_w - totalButtonWidth) * 0.5f;
            ImGui::SetCursorPosX(cursorX);
    
            // Play Button
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1f, 0.1f, 0.8f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2f, 0.2f, 1.0f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.0f, 0.0f, 0.6f, 1.0f));
            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 2.5f));
            if (ImGui::Button("Play", ImVec2(button_size.x,button_size.y)))
            {
                glfwSetInputMode(m_Window->GetGLFWwindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                SetDisableInput(false);
                SetShowMyWindow(false);
            }
            ImGui::SameLine();
            ImGui::PopStyleVar(2);
            ImGui::PopStyleColor(3);
            
            // Style and exit button styling
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.1f, 0.1f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.2f, 0.2f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.6f, 0.0f, 0.0f, 1.0f));
            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 2.5f));
            if (ImGui::Button("Exit", ImVec2(button_size.x,button_size.y)))
            {
                Exit();
                return;
            }
            ImGui::PopStyleVar(2);
            ImGui::PopStyleColor(3);

            ImGui::End();
        }
    
        // ===================================================
        // Right Panel: Scene Settings
        // ===================================================
        if (m_ShowMyWindow)
        {
            float panel_width = 300.0f;
            // Get the main viewport's work area
            const ImGuiViewport* viewport = ImGui::GetMainViewport();
            // Set position: right side of the viewport (work area) and at the top
            ImGui::SetNextWindowPos
            (
                ImVec2(viewport->WorkPos.x + viewport->WorkSize.x - panel_width, viewport->WorkPos.y),
                ImGuiCond_Always
            );
            // Set size: fixed panel width, and full height of the viewport work area
            ImGui::SetNextWindowSize
            (
                ImVec2(panel_width, viewport->WorkSize.y),
                ImGuiCond_Always
            );

            ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDocking       | 
                                           ImGuiWindowFlags_NoCollapse      | 
                                           ImGuiWindowFlags_NoDecoration;

            ImGui::Begin("Scene Settings", nullptr, windowFlags);

            // Display FPS.
            ImGui::Text("FPS: %.4f", m_FPS);

            if (ImGui::CollapsingHeader("Environment Settings"))
            {
                ImGui::Text("Background Color");
                ImGui::ColorEdit3("Background Color", (float *)&m_BackgroundColor, ImGuiColorEditFlags_NoLabel);
                if (ImGui::Button("Reset Background Color"))
                {
                    m_BackgroundColor = {0.0f, 0.0f, 0.0f};
                    ImGui::SetTooltip("Reset background color to default.");
                }
            }

            if (ImGui::CollapsingHeader("Camera Settings"))
            {
                ImGui::Text("Camera Movement Speed");
                ImGui::DragFloat("##", &m_Camera->GetSpeed(), 0.01f, 0.1f, 10.0f, "%.2f");
                if (ImGui::Button("Reset Camera Speed"))
                {
                    m_Camera->ResetSpeed();
                    ImGui::SetTooltip("Reset camera speed to initial value.");
                }
                ImGui::SameLine();
                if (ImGui::Button("Reset Camera Position"))
                {
                    m_Camera->ResetPosition();
                    ImGui::SetTooltip("Reset camera to default position.");
                }

                ImGui::Text("Camera Zoom");
                ImGui::DragFloat("##", &m_Camera->GetZoom(), 0.1f, 1.0f, 45.0f, "%.2f");
                if (ImGui::Button("Reset Camera Zoom"))
                {
                    m_Camera->SetPosition(glm::vec3(0.0f, 0.0f, 3.0f));
                    ImGui::SetTooltip("Reset camera zoom to default (45.f).");
                }
            }

            if (ImGui::CollapsingHeader("Mouse Settings"))
            {
                auto& mouse = Mouse::GetInstance();
                ImGui::Text("Mouse Sensitivity");
                ImGui::DragFloat("##", &mouse.GetSensitivity(), 0.01f, 0.01f, 5.0f, "%.4f");
                if (ImGui::Button("Reset Mouse Sensitivity"))
                {
                    mouse.ResetSensitivity();
                    ImGui::SetTooltip("Reset mouse sensitivity.");
                }
            }

            if (ImGui::CollapsingHeader("Cube Settings"))
            {
                ImGui::Text("Cube Count: %zu", GetCubeCount());
                static float cubePosition[3] = {0.0f, 0.0f, 0.0f};
                ImGui::Text("Position");
                ImGui::InputFloat3("##", cubePosition);
                if (ImGui::Button("Add Cube at Position"))
                {
                    AddCube(glm::vec3(cubePosition[0], cubePosition[1], cubePosition[2]));
                }
                static int addAmount = 1;
                ImGui::Text("Add Amount");
                ImGui::InputInt("##", &addAmount);
                addAmount = std::max(1, addAmount);
                if (ImGui::Button("Add Multiple Cubes"))
                {
                    AddCubes(addAmount);
                }
                if (ImGui::Button("Remove All Cubes"))
                {
                    ClearCubes();
                }
                if (ImGui::CollapsingHeader("Active Cubes"))
                {
                    for (size_t i = 0; i < m_Cubes.size(); ++i)
                    {
                        ImGui::PushID(static_cast<int>(i));
                        ImGui::Text("Cube %zu", i + 1);
                        ImGui::Text("Position");
                        glm::vec3& position = m_Cubes[i]->GetPosition();
                        if (ImGui::DragFloat3("##", &position[0], 0.1f, -1000.0f, 1000.0f))
                        {
                            m_Cubes[i]->SetPosition(position);
                        }
                        if (ImGui::Button("Remove Cube"))
                        {
                            RemoveCube(i);
                            ImGui::PopID();
                            break;
                        }
                        ImGui::Separator();
                        ImGui::PopID();
                    }
                }
            }

            if (ImGui::CollapsingHeader("Light Cube Settings"))
            {
                ImGui::Text("Light Cube Position");
                ImGui::DragFloat3("##", (float *)&m_Light->GetPosition(), 0.01f, -1000.f, 1000.f, "%.4f");
                ImGui::Text("Light Cube Color");
                ImGui::ColorEdit3("##", (float *)&m_Light->GetColor(), ImGuiColorEditFlags_NoLabel);
                ImGui::Text("Specular Intensity");
                ImGui::DragFloat("##", &m_Light->GetSpecularIntensity(), 0.01f, 0.0f, 1.0f, "%.4f");
            }

            ImGui::End();
        }

        // Finalize ImGui rendering
        ImGui::End();
        ImGui::Render();
    }


    void Engine::ImguiCenterItem(float itemWidth)
    {
        float windowWidth = ImGui::GetContentRegionAvail().x;
        ImGui::SetCursorPosX((windowWidth - itemWidth) * 0.5f); // Set the cursor position to the middle
    }

    void Engine::ImguiSetCustomColorStyle()
    {
        ImGuiStyle& style = ImGui::GetStyle();
        ImVec4* colors = style.Colors;

        // Base style adjustments
        style.FrameRounding = 5.0f;
        style.FramePadding = ImVec2(5.0f, 3.0f);
        style.WindowRounding = 5.0f;
        style.ScrollbarRounding = 5.0f;
        style.GrabRounding = 5.0f;

        // Base colors
        // Let's go with a dark grey background, blue highlights, and lighter elements.
        colors[ImGuiCol_WindowBg]            = IMGUI_COLOR_DARKGREY;
        colors[ImGuiCol_ChildBg]             = IMGUI_COLOR_DARKGREY;
        colors[ImGuiCol_PopupBg]             = IMGUI_COLOR_DARKGREY;
        colors[ImGuiCol_Border]              = IMGUI_COLOR_GREY;
        colors[ImGuiCol_BorderShadow]        = IMGUI_COLOR_BLACK;

        // Text
        colors[ImGuiCol_Text]                = IMGUI_COLOR_WHITE;
        colors[ImGuiCol_TextDisabled]        = IMGUI_COLOR_GREY;

        // Headers (for collapsing headers)
        colors[ImGuiCol_Header]              = IMGUI_COLOR_BLUE;
        colors[ImGuiCol_HeaderHovered]       = IMGUI_COLOR_PURPLE;  
        colors[ImGuiCol_HeaderActive]        = IMGUI_COLOR_DARKPURPLE;

        // Buttons
        colors[ImGuiCol_Button]              = IMGUI_COLOR_DARKBLUE;
        colors[ImGuiCol_ButtonHovered]       = IMGUI_COLOR_BLUE;
        colors[ImGuiCol_ButtonActive]        = IMGUI_COLOR_DARKPURPLE;

        // Frame BG (used for checkbox, radio button, frame around text input fields, etc.)
        colors[ImGuiCol_FrameBg]             = IMGUI_COLOR_DARKGREY;
        colors[ImGuiCol_FrameBgHovered]      = IMGUI_COLOR_BLUEGREY;
        colors[ImGuiCol_FrameBgActive]       = IMGUI_COLOR_PURPLE;

        // Sliders
        // Make the slider background distinct from the slider handle:
        colors[ImGuiCol_SliderGrab]          = IMGUI_COLOR_BLUE;       // Slider handle
        colors[ImGuiCol_SliderGrabActive]    = IMGUI_COLOR_PURPLE;    // Slider handle when active
        // The "frame" around slider is FrameBg, already set to a darker color (IMGUI_COLOR_DARKGREY)

        // Checkmarks and other widgets
        colors[ImGuiCol_CheckMark]           = IMGUI_COLOR_GREEN;
        
        // Scrollbar
        colors[ImGuiCol_ScrollbarBg]         = IMGUI_COLOR_DARKGREY;
        colors[ImGuiCol_ScrollbarGrab]       = IMGUI_COLOR_GREY;
        colors[ImGuiCol_ScrollbarGrabHovered]= IMGUI_COLOR_BLUE;
        colors[ImGuiCol_ScrollbarGrabActive] = IMGUI_COLOR_PURPLE;

        // Tabs (if you use docking)
        colors[ImGuiCol_Tab]                 = IMGUI_COLOR_DARKGREY;
        colors[ImGuiCol_TabHovered]          = IMGUI_COLOR_BLUE;
        colors[ImGuiCol_TabActive]           = IMGUI_COLOR_PURPLE;
        colors[ImGuiCol_TabUnfocused]        = IMGUI_COLOR_DARKGREY;
        colors[ImGuiCol_TabUnfocusedActive]  = IMGUI_COLOR_GREY;

        // Title bar
        colors[ImGuiCol_TitleBg]             = IMGUI_COLOR_DARKGREY;
        colors[ImGuiCol_TitleBgActive]       = IMGUI_COLOR_BLUE;
        colors[ImGuiCol_TitleBgCollapsed]    = IMGUI_COLOR_DARKGREY;

        // Separator
        colors[ImGuiCol_Separator]           = IMGUI_COLOR_GREY;
        colors[ImGuiCol_SeparatorHovered]    = IMGUI_COLOR_BLUE;
        colors[ImGuiCol_SeparatorActive]     = IMGUI_COLOR_PURPLE;

        // Resize grip
        colors[ImGuiCol_ResizeGrip]          = IMGUI_COLOR_GREY;
        colors[ImGuiCol_ResizeGripHovered]   = IMGUI_COLOR_BLUE;
        colors[ImGuiCol_ResizeGripActive]    = IMGUI_COLOR_PURPLE;

        // Plotting (if needed)
        colors[ImGuiCol_PlotLines]           = IMGUI_COLOR_BLUE;
        colors[ImGuiCol_PlotLinesHovered]    = IMGUI_COLOR_PURPLE;
        colors[ImGuiCol_PlotHistogram]       = IMGUI_COLOR_GREEN;
        colors[ImGuiCol_PlotHistogramHovered]= IMGUI_COLOR_PURPLE;

        // Modal Window Darkening
        colors[ImGuiCol_ModalWindowDimBg]    = IMGUI_COLOR_BLACK;
    }

    void Engine::AddCubes(int addAmount, float minRange, float maxRange)
    {
        for (int i = 0; i < addAmount; ++i)
        {
            float randomX = static_cast<float>(rand()) / RAND_MAX * (maxRange - minRange) + minRange;
            float randomY = static_cast<float>(rand()) / RAND_MAX * (maxRange - minRange) + minRange;
            float randomZ = static_cast<float>(rand()) / RAND_MAX * (maxRange - minRange) + minRange;

            glm::vec3 randomPosition(randomX, randomY, randomZ);
            Cube* newCube = new Cube(randomPosition);
            m_Cubes.push_back(newCube);
        }
    }

    // -----------------------------------------------------------------------------------------------------
    // GLFW Callback Functions
    // -----------------------------------------------------------------------------------------------------

    void FramebufferSizeCallback(GLFWwindow *window, int width, int height)
    {
        glViewport(0, 0, width, height);
    }

    void MouseCallback( GLFWwindow *window, double xposIn, double yposIn)
    {
        float xpos = static_cast<float>(xposIn);
        float ypos = static_cast<float>(yposIn);

        auto engine = Engine::GetInstance();
        auto&  mouse = Mouse::GetInstance();

        auto &first_mouse = mouse.GetFirstMouse();
        auto &last_x = mouse.GetLastX();
        auto &last_y = mouse.GetLastY();

        if (first_mouse)
        {
            last_x = xpos;
            last_y = ypos;
            first_mouse = false;
        }

        float xoffset = xpos - last_x;
        float yoffset = last_y - ypos; // reversed since y-coordinates go from bottom to top

        last_x = xpos;
        last_y = ypos;

        if (!engine->GetDisableInput())
        {
            auto camera = engine->GetCamera();
            mouse.ProcessMovement(camera,xoffset, yoffset);
        }
    }

    // glfw: whenever the mouse scroll wheel scrolls, this callback is called
    // ----------------------------------------------------------------------
    void ScrollCallback([[maybe_unused]] GLFWwindow *window,
                        [[maybe_unused]] double xoffset,
                        double yoffset)
    {
        auto&  mouse = Mouse::GetInstance();
        auto  engine = Engine::GetInstance();
        mouse.ProcessScroll(static_cast<float>(yoffset),engine->GetCamera());
    }

    void KeyCallback(GLFWwindow *window, int key, [[maybe_unused]] int scancode, int action, [[maybe_unused]] int mods)
    {
        /*
        auto engine = Engine::GetInstance();
        if (key == GLFW_KEY_H && action == GLFW_PRESS && !engine->GetKeyPressed())
        {
            engine->SetKeyPressed(true);
            if (!engine->GetDisableInput())
            {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                engine->SetDisableInput(true);
                engine->SetShowMyWindow(true);
            }
            else
            {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                engine->SetDisableInput(false);
                engine->SetShowMyWindow(false);
            }
        }

        if (key == GLFW_KEY_H && action == GLFW_RELEASE)
        {
            engine->SetKeyPressed(false);
        }
        
        */
    }

    // -----------------------------------------------------------------------------------------------------

    // -----------------------------------------------------------------------------------------------------
    // More Imgui Stuff
    // -----------------------------------------------------------------------------------------------------

    // Helper to display a little (?) mark which shows a tooltip when hovered.
    // In your own code you may want to display an actual icon if you are using a merged icon fonts (see
    // docs/FONTS.md)
    void HelpMarker(const char *desc)
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
        ImGuiIO &io = ImGui::GetIO();
        ImGui::Text("ERROR: Docking is not enabled! See Demo > Configuration.");
        ImGui::Text("Set io.ConfigFlags |= ImGuiConfigFlags_DockingEnable in your code, or ");
        ImGui::SameLine(0.0f, 0.0f);
        if (ImGui::SmallButton("click here"))
            io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    }

    void SetImguiDocking(bool *p_open)
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

        static bool opt_fullscreen = true;
        static bool opt_padding = false;
        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None | ImGuiDockNodeFlags_PassthruCentralNode;

        // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
        // because it would be confusing to have two docking targets within each others.
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        if (opt_fullscreen)
        {
            const ImGuiViewport *viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->WorkPos);
            ImGui::SetNextWindowSize(viewport->WorkSize);
            ImGui::SetNextWindowViewport(viewport->ID);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
            window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
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
        ImGuiIO &io = ImGui::GetIO();
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
                                    (dockspace_flags & ImGuiDockNodeFlags_NoDockingOverCentralNode) != 0))
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
    // -----------------------------------------------------------------------------------------------------

} // namespace isaacGraphicsEngine
