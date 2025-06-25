#include "ImGuiLayer.h"
#include "../Core/Engine.h" // so we can access engine state like m_Camera, m_Light, etc.
#include "../Core/Mouse.h"
#include <imgui_internal.h>

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
           
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);

        ImGuiWindowFlags host_flags =
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoBringToFrontOnFocus |
            ImGuiWindowFlags_NoNavFocus |
            ImGuiWindowFlags_NoBackground;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0,0));
        ImGui::Begin("MainDockSpaceHost", nullptr, host_flags);

        ImGuiID dockspace_id = ImGui::GetID("MainDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0,0), ImGuiDockNodeFlags_PassthruCentralNode);

        ImGui::End();
        ImGui::PopStyleVar();

        // should only run once
        static bool first_time = true;
        if (first_time)
        {
            first_time = false;
            ImGui::DockBuilderRemoveNode(dockspace_id);
            ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
            ImGui::DockBuilderSetNodeSize(dockspace_id, ImGui::GetMainViewport()->Size);

            // Split right sidebar (fixed width)
            ImGuiID right_id, main_id;
            float rightPanelWidth = 300.0f; // or whatever you want
            ImGui::DockBuilderSplitNode(
                dockspace_id, ImGuiDir_Right,
                rightPanelWidth / ImGui::GetMainViewport()->Size.x, // fractional ratio
                &right_id, &main_id);

            // Split main area into top bar and viewport
            ImGuiID top_id, viewport_id;
            float topPanelHeight = 40.0f; // px, match your panel height
            ImGui::DockBuilderSplitNode(
                main_id, ImGuiDir_Up,
                topPanelHeight / ImGui::GetMainViewport()->Size.y,
                &top_id, &viewport_id);

            // Split right sidebar vertically for two panels
            ImGuiID sceneHierarchy_id, sceneSettings_id;
            ImGui::DockBuilderSplitNode(
                right_id, ImGuiDir_Up, 0.25f, &sceneHierarchy_id, &sceneSettings_id);

            // Dock windows
            ImGui::DockBuilderDockWindow("Engine Controls", top_id);
            ImGui::DockBuilderDockWindow("Viewport", viewport_id);
            ImGui::DockBuilderDockWindow("Scene Hierarchy Panel", sceneHierarchy_id);
            ImGui::DockBuilderDockWindow("Scene Settings", sceneSettings_id);

            ImGui::DockBuilderFinish(dockspace_id);
        }


        DrawTopPanel(engine);
        DrawRightPanel(engine);
        DrawSceneHierarchyPanel(engine);
        
        ImGui::Render();
    }

    void ImGuiLayer::DrawTopPanel(Engine* engine)
    {
        // ===================================================
        // Top Control Panel 
        // ===================================================
    
        ImGuiWindowClass window_class;
        window_class.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoTabBar;
        ImGui::SetNextWindowClass(&window_class);

        // No docking, collapse, or moving; and hide title bar for a clean look.
        ImGuiWindowFlags topPanelFlags = 
            ImGuiWindowFlags_NoMove | 
            ImGuiWindowFlags_NoDecoration;
        
        ImGui::Begin("Engine Controls", nullptr, topPanelFlags);
        
        // draw the button with a little ▼ glyph
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
        /*
        bool shouldExit = false;
        if (ImGui::Button("Exit", ImVec2(button_size.x,button_size.y)))
        {
            shouldExit = true;
        }
        */
        ImGui::PopStyleVar(2);
        ImGui::PopStyleColor(3);

        ImGui::End();

        /*
        if (shouldExit)
        {
            engine->Exit();
            return;
        }
        */
    }

    void ImGuiLayer::DrawRightPanel(Engine* engine)
    {
        // ===================================================
        // Right Panel: Scene Settings
        // ===================================================
        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove;
        
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
            auto* mouse = Mouse::GetInstance();
            ImGui::Text("Mouse Sensitivity");
            ImGui::DragFloat("##Mouse Sensitivity", &mouse->GetSensitivity(), 0.01f, 0.01f, 5.0f, "%.4f");
            if (ImGui::Button("Reset Mouse Sensitivity"))
            {
                mouse->ResetSensitivity();
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

        if (ImGui::CollapsingHeader("Light Settings"))
        {    
            if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_Leaf))
            {
                if (ImGui::BeginTable("TransformTable", 2)) 
                {                
                    ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, 80.0f);
                    ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);

                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0); ImGui::Text("Position");
                    ImGui::TableSetColumnIndex(1); ImGui::DragFloat3("##Light Position", (float *)&engine->GetLight()->GetPosition(), 0.01f);
                    
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0); ImGui::Text("Rotation");
                    ImGui::TableSetColumnIndex(1); ImGui::DragFloat3("##Light Rotation", (float *)&engine->GetLight()->GetRotation(), 0.01f);
                    
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0); ImGui::Text("Scale");
                    ImGui::TableSetColumnIndex(1); ImGui::DragFloat3("##Light Scale", (float *)&engine->GetLight()->GetScale(), 0.01f);
                    
                    ImGui::EndTable();
                }
            }
            ImGui::ColorEdit3("Color", (float *)&engine->GetLight()->GetColor(), ImGuiColorEditFlags_NoLabel);
            ImGui::Text("Specular Intensity");
            ImGui::DragFloat("##Specular Intensity", &engine->GetLight()->GetSpecularIntensity(), 0.01f);
        }

        ImGui::End();
    }

    void ImGuiLayer::DrawSceneHierarchyPanel(Engine* engine)
    {
        // ===================================================
        // Scene Hierarchy Panel
        // ===================================================
        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove;

        ImGui::Begin("Scene Hierarchy Panel", nullptr, windowFlags);
                    
        ImGui::Text("FPS: %.4f", engine->GetFPS());

        ImGui::End();
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