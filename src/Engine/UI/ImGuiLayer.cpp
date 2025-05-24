#include "ImGuiLayer.h"
#include "../Core/Engine.h" // so we can access engine state like m_Camera, m_Light, etc.
#include "../Core/Mouse.h"

namespace isaacObjectLoader
{
 
 
    ImGuiLayer::ImGuiLayer()
    {

    }

    ImGuiLayer::~ImGuiLayer()
    {
        Shutdown();
    }

    void ImGuiLayer::Init(SDL_Window* window, void* gl_context)
    {
        m_Window = window;
        m_GLContext = gl_context;

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking

        ImGui::StyleColorsDark();

        // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
        ImGuiStyle& style = ImGui::GetStyle();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            style.WindowRounding = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }
        
        ImGui_ImplSDL3_InitForOpenGL(window, gl_context);
        ImGui_ImplOpenGL3_Init("#version 330 core");
        
        LoadFont();
    }
    
    void ImGuiLayer::Begin()
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();
    }
    
    void ImGuiLayer::End()
    {
        ImGuiIO& io = ImGui::GetIO();
        ImGui::Render();
        glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
        /*glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);*/
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Update and Render additional Platform Windows
        // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
        //  For this specific demo app we could also call SDL_GL_MakeCurrent(window, gl_context) directly)
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            SDL_Window* backup_current_window = SDL_GL_GetCurrentWindow();
            SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
        }
    }
    
    void ImGuiLayer::Shutdown()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL3_Shutdown();
        ImGui::DestroyContext();
    }

    void ImGuiLayer::DrawUI()
    {
        Engine* engine = Engine::GetInstance();

        //ImguiSetCustomColorStyle();
        
        DrawTopPanel(engine);
        DrawRightPanel(engine);
    }

    void ImGuiLayer::DrawTopPanel(Engine* engine)
    {
        // ===================================================
        // Top Control Panel 
        // ===================================================
        
        int display_w, display_h;
        SDL_GetWindowSizeInPixels(engine->GetSDLWindow(), &display_w, &display_h);

        int button_num = 3;
        glm::vec2 button_size = {40,20}; // in px
        int spacing = 20;
        
        // Fixed height for the control panel (e.g., 50 pixels)
        float controlPanelHeight = 20.0f;
        ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(display_w - m_PanelWidth, controlPanelHeight), ImGuiCond_Always);
        
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
        
        // draw the button with a little ▼ glyph so it *looks* like a dropdown
        if (ImGui::Button(u8"Modes \uf0d7"))
            ImGui::OpenPopup("modes_popup");         // toggle the popup

        if (ImGui::BeginPopup("modes_popup"))
        {
            if (ImGui::MenuItem("Mouse mode"))       // behaves like a <li>
            {
                engine->EnableMouseMode();
            }

            if (ImGui::MenuItem("Free camera mode"))
            {
                engine->EnableFreeCameraMode();
            }

            ImGui::EndPopup();
        }

        // Style and exit button styling
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.1f, 0.1f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.2f, 0.2f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.6f, 0.0f, 0.0f, 1.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 2.5f));
        bool shouldExit = false;
        if (ImGui::Button("Exit", ImVec2(button_size.x,button_size.y)))
        {
            shouldExit = true;
        }
        ImGui::PopStyleVar(2);
        ImGui::PopStyleColor(3);

        ImGui::End();

        if (shouldExit)
        {
            engine->Exit();
            return;
        }
    }

    void ImGuiLayer::DrawRightPanel(Engine* engine)
    {
        // ===================================================
        // Right Panel: Scene Settings
        // ===================================================
            // Get the main viewport's work area
            const ImGuiViewport* viewport = ImGui::GetMainViewport();
            // Set position: right side of the viewport (work area) and at the top
            ImGui::SetNextWindowPos
            (
                ImVec2(viewport->WorkPos.x + viewport->WorkSize.x - m_PanelWidth, viewport->WorkPos.y),
                ImGuiCond_Always
            );
            // Set size: fixed panel width, and full height of the viewport work area
            ImGui::SetNextWindowSize
            (
                ImVec2(m_PanelWidth, viewport->WorkSize.y),
                ImGuiCond_Always
            );

            ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDocking       | 
                                           ImGuiWindowFlags_NoCollapse      | 
                                           ImGuiWindowFlags_NoDecoration;

            ImGui::Begin("Scene Settings", nullptr, windowFlags);

            // Display FPS.
            ImGui::Text("FPS: %.4f", engine->GetFPS());

            if (ImGui::CollapsingHeader("Environment Settings"))
            {
                ImGui::Text("Background Color");
                ImGui::ColorEdit3("##Background Color", (float *)&engine->GetBackgroundColor(), ImGuiColorEditFlags_NoLabel);
                if (ImGui::Button("Reset Background Color"))
                {
                    engine->SetBackgroundColor({0.0f, 0.0f, 0.0f});
                    ImGui::SetTooltip("Reset background color to default.");
                }
            }

            if (ImGui::CollapsingHeader("Camera Settings"))
            {
                ImGui::Text("Camera Movement Speed");
                ImGui::DragFloat("##Camera Movement Speed", &engine->GetCamera()->GetSpeed(), 0.01f, 0.1f, 10.0f, "%.2f");
                if (ImGui::Button("Reset Camera Speed"))
                {
                    engine->GetCamera()->ResetSpeed();
                    ImGui::SetTooltip("Reset camera speed to initial value.");
                }
                ImGui::SameLine();
                if (ImGui::Button("Reset Camera Position"))
                {
                    engine->GetCamera()->ResetPosition();
                    ImGui::SetTooltip("Reset camera to default position.");
                }

                ImGui::Text("Camera Zoom");
                ImGui::DragFloat("##Camera Zoom", &engine->GetCamera()->GetZoom(), 0.1f, 1.0f, 45.0f, "%.2f");
                if (ImGui::Button("Reset Camera Zoom"))
                {
                    engine->GetCamera()->SetPosition(glm::vec3(0.0f, 0.0f, 3.0f));
                    ImGui::SetTooltip("Reset camera zoom to default (45.f).");
                }
            }

            if (ImGui::CollapsingHeader("Mouse Settings"))
            {
                auto& mouse = Mouse::GetInstance();
                ImGui::Text("Mouse Sensitivity");
                ImGui::DragFloat("##Mouse Sensitivity", &mouse.GetSensitivity(), 0.01f, 0.01f, 5.0f, "%.4f");
                if (ImGui::Button("Reset Mouse Sensitivity"))
                {
                    mouse.ResetSensitivity();
                    ImGui::SetTooltip("Reset mouse sensitivity.");
                }
            }

            if (ImGui::CollapsingHeader("Cube Settings"))
            {
                ImGui::Text("Cube Count: %zu", engine->GetCubeCount());
                static float cubePosition[3] = {0.0f, 0.0f, 0.0f};
                ImGui::Text("Position");
                ImGui::InputFloat3("##Position", cubePosition);
                if (ImGui::Button("Add Cube at Position"))
                {
                    engine->AddCube(glm::vec3(cubePosition[0], cubePosition[1], cubePosition[2]));
                }
                static int addAmount = 1;
                ImGui::Text("Add Amount");
                ImGui::InputInt("##Add Amount", &addAmount);
                addAmount = std::max(1, addAmount);
                if (ImGui::Button("Add Multiple Cubes"))
                {
                    engine->AddCubes(addAmount);
                }
                if (ImGui::Button("Remove All Cubes"))
                {
                    engine->ClearCubes();
                }
                if (ImGui::CollapsingHeader("Active Cubes"))
                {
                    for (size_t i = 0; i < engine->GetCubeCount(); ++i)
                    {
                        ImGui::PushID(static_cast<int>(i));
                        ImGui::Text("Cube %zu", i + 1);
                        ImGui::Text("Position");
                        glm::vec3& position = engine->GetCube(i)->GetPosition();
                        if (ImGui::DragFloat3("##Position", &position[0], 0.1f, -1000.0f, 1000.0f))
                        {
                            engine->GetCube(i)->SetPosition(position);
                        }
                        if (ImGui::Button("Remove Cube"))
                        {
                            engine->RemoveCube(i);
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
                ImGui::DragFloat3("##Light Cube Position", (float *)&engine->GetLight()->GetPosition(), 0.01f, -1000.f, 1000.f, "%.4f");
                ImGui::Text("Light Cube Color");
                ImGui::ColorEdit3("##Light Cube Color", (float *)&engine->GetLight()->GetColor(), ImGuiColorEditFlags_NoLabel);
                ImGui::Text("Specular Intensity");
                ImGui::DragFloat("##Specular Intensity", &engine->GetLight()->GetSpecularIntensity(), 0.01f, 0.0f, 1.0f, "%.4f");
            }

        // Finalize ImGui rendering
        ImGui::End();
        ImGui::Render();
    }

    void ImGuiLayer::LoadFont()
    {
        ImGuiIO& io = ImGui::GetIO();

        ImFontConfig cfg{};
        cfg.PixelSnapH = true;

        std::string path = GetProjectRoot() +
                        "/src/Resources/FiraCodeNerdFontMono-Regular.ttf";
        
        static const ImWchar puaRanges[] = {
            0x0020, 0x00FF,   // keep your ASCII
            0xE000, 0xF8FF,   // all PUA (where Nerd-Font patches in icons)
            0
        };

        ImFont* nerd = io.Fonts->AddFontFromFileTTF(
            path.c_str(), 16.0f, &cfg, puaRanges
        );
        io.FontDefault = nerd;
        ImGui_ImplOpenGL3_DestroyFontsTexture();
        ImGui_ImplOpenGL3_CreateFontsTexture();
    }


}