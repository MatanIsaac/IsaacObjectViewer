#include "ImGuiLayer.h"
#include "../Core/Engine.h" // so we can access engine state like m_Camera, m_Light, etc.
#include "../Core/Mouse.h"

namespace isaacObjectViewer
{
 
 
    ImGuiLayer::ImGuiLayer()
    {
        m_GizmoOperation = ImGuizmo::TRANSLATE;
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
        ImGuizmo::BeginFrame();
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

            ImGuiID right_id, main_id;
            float rightPanelWidth = 350.0f; 
            ImGui::DockBuilderSplitNode(
                dockspace_id, ImGuiDir_Right,
                rightPanelWidth / ImGui::GetMainViewport()->Size.x, 
                &right_id, &main_id);

            ImGuiID top_id, viewport_id;
            float topPanelHeight = 40.0f; 
            ImGui::DockBuilderSplitNode(
                main_id, ImGuiDir_Up,
                topPanelHeight / ImGui::GetMainViewport()->Size.y,
                &top_id, &viewport_id);

            ImGuiID sceneHierarchy_id, sceneSettings_id;
            ImGui::DockBuilderSplitNode(
                right_id, ImGuiDir_Up, 0.25f, &sceneHierarchy_id, &sceneSettings_id);

            ImGui::DockBuilderDockWindow("Engine Controls", top_id);
            ImGui::DockBuilderDockWindow("Viewport", viewport_id);
            ImGui::DockBuilderDockWindow("Scene Hierarchy Panel", sceneHierarchy_id);
            ImGui::DockBuilderDockWindow("Scene Settings", sceneSettings_id);

            ImGui::DockBuilderFinish(dockspace_id);
        }

        DrawTopPanel(engine);
        DrawSceneHierarchyPanel(engine);
        DrawRightPanel(engine);
        
        DrawGizmos(engine, m_GizmoOperation);
        
        ImGui::Render();
    }

    void ImGuiLayer::DrawTopPanel(Engine* engine)
    {
        // ===================================================
        // Top Control Panel 
        // ===================================================
        
        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("New")) { /* New file action */ }
                if (ImGui::MenuItem("Open...")) { /* Open file action */ }
                if (ImGui::MenuItem("Save")) { /* Save file action */ }
                ImGui::Separator();
                if (ImGui::MenuItem("Exit")) 
                { 
                    
                }
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Add"))
            {
                if (ImGui::BeginMenu("Primitive Objects")) 
                {
                    if (ImGui::MenuItem("Cube"))   
                    {
                        engine->AddSceneObject(ObjectType::Cube);
                    }
                    if (ImGui::MenuItem("Sphere")) 
                    {
                        engine->AddSceneObject(ObjectType::Sphere);
                    }
                    if (ImGui::MenuItem("Cylinder")) 
                    { 
                        engine->AddSceneObject(ObjectType::Cylinder);
                    }
                    if (ImGui::MenuItem("Plane")) 
                    { 
                        engine->AddSceneObject(ObjectType::Plane);
                    }
                    
                    ImGui::EndMenu();
                }
                
                if (ImGui::BeginMenu("Lights")) 
                {
                    if (ImGui::MenuItem("PointLight"))   
                    {
                        engine->AddSceneObject(ObjectType::PointLight);
                    }
                    ImGui::EndMenu();
                }
                
                ImGui::EndMenu();
            }

            ImGui::EndMainMenuBar();
        }

        ImGuiWindowClass window_class;
        window_class.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoTabBar;
        ImGui::SetNextWindowClass(&window_class);

        // No docking, collapse, or moving; and hide title bar for a clean look.
        ImGuiWindowFlags topPanelFlags = 
            ImGuiWindowFlags_NoMove | 
            ImGuiWindowFlags_NoDecoration;
        
        ImGui::Begin("Engine Controls", nullptr, topPanelFlags);
        
        ImGui::Text("Gizmo Mode:");

        ImGui::SameLine();
        ImGui::RadioButton("Translate", &m_GizmoOperation, ImGuizmo::TRANSLATE); ImGui::SameLine();
        ImGui::RadioButton("Rotate", &m_GizmoOperation, ImGuizmo::ROTATE); ImGui::SameLine();
        ImGui::RadioButton("Scale", &m_GizmoOperation, ImGuizmo::SCALE);
        ImGui::SameLine();
        // draw the button with a little ▼ glyph
        if (ImGui::Button(u8"View Modes \uf0d7"))
        ImGui::OpenPopup("modes_popup");         // toggle the popup
        
        if (ImGui::BeginPopup("modes_popup"))
        {
            if (ImGui::MenuItem("Mouse mode"))       
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
        ImGui::PopStyleVar(2);
        ImGui::PopStyleColor(3);

        ImGui::End();

    }

    void ImGuiLayer::DrawRightPanel(Engine* engine)
    {
        // ===================================================
        // Right Panel: Scene Settings
        // ===================================================

        int display_w, display_h;
        SDL_GetWindowSizeInPixels(engine->GetSDLWindow(), &display_w, &display_h);

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
        
        ImGui::Separator();

        if (ImGui::CollapsingHeader("Camera Settings"))
        {
            auto camera = engine->GetCamera();
            
            if (ImGui::BeginTable("TransformTable", 2)) 
            {                
                ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, 80.0f);
                ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::Text("Position");
                ImGui::TableSetColumnIndex(1); ImGui::DragFloat3("##Position", (float *)&camera->GetPosition(), 0.01f);
                            
                ImGui::EndTable();
            }
            
            if (ImGui::Button("Reset Camera Position"))
            {
                engine->GetCamera()->ResetPosition();
                ImGui::SetTooltip("Reset camera to default position.");
            }

            ImGui::Text("Camera Movement Speed");
            ImGui::DragFloat("##Camera Movement Speed", &engine->GetCamera()->GetSpeed(), 0.01f, 0.1f, 10.0f, "%.2f");
            if (ImGui::Button("Reset Camera Speed"))
            {
                engine->GetCamera()->ResetSpeed();
                ImGui::SetTooltip("Reset camera speed to initial value.");
            }

            ImGui::Text("Camera Zoom");
            if(ImGui::DragFloat("##Camera Zoom", &engine->GetCamera()->GetZoom(), 0.1f, 1.0f, 90.0f, "%.2f"))
            {
                engine->GetCamera()->SetProjection((float)display_w / (float)display_h, 0.1f,100.0f);
            }
            if (ImGui::Button("Reset Camera Zoom"))
            {
                engine->GetCamera()->SetZoom(engine->GetCamera()->DEFAULT_CAMERA_ZOOM);
                engine->GetCamera()->SetProjection((float)display_w / (float)display_h, 0.1f,100.0f);
                ImGui::SetTooltip("Reset camera zoom to default (45.f).");
            }
        }
        
        ImGui::Separator();
        
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

        ImGui::Separator();
        
        if(selected)
        {
            if (ImGui::CollapsingHeader("Settings",ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Leaf))
            {    
                if (ImGui::CollapsingHeader("Transform"))
                {
                    if (ImGui::BeginTable("TransformTable", 2)) 
                    {                
                        ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, 80.0f);
                        ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);

                        ImGui::TableNextRow();
                        ImGui::TableSetColumnIndex(0); ImGui::Text("Position");
                        ImGui::TableSetColumnIndex(1); ImGui::DragFloat3("##Position", (float *)&selected->GetPosition(), 0.01f);
                        
                        ImGui::TableNextRow();
                        ImGui::TableSetColumnIndex(0); ImGui::Text("Rotation");
                        ImGui::TableSetColumnIndex(1); 
                        if(ImGui::DragFloat3("##Rotation", (float *)&selected->GetRotation(), 0.01f))
                        {
                            selected->SetOrientation(glm::quat(glm::radians(selected->GetRotation())));
                        }
                        
                        ImGui::TableNextRow();
                        ImGui::TableSetColumnIndex(0); ImGui::Text("Scale");
                        ImGui::TableSetColumnIndex(1); ImGui::DragFloat3("##Scale", (float *)&selected->GetScale(), 0.01f);
                        
                        ImGui::EndTable();
                    }
                }
                DrawSettings(selected);
                  
            }
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
                    
        auto sceneObjects = engine->GetSceneObjects();
        selected = engine->GetSelectedObject();

        for (size_t i = 0; i < sceneObjects.size(); ++i) 
        {
            ISceneObject* obj = sceneObjects[i];
            ImGui::PushID(obj->GetID());                        // push a unique ID for this object (e.g., index)
            bool isSelected = false;
            if(selected)
                isSelected = (obj == selected);
            std::string label = "##" + obj->GetName();
            if (ImGui::Selectable(label.c_str(), isSelected,ImGuiSelectableFlags_AllowDoubleClick)) 
            { 
                engine->SetSelectedObject(obj);
            }
            // draw the actual object name as text next to the selectable
            ImGui::SameLine();
            ImGui::TextUnformatted(obj->GetName().c_str());
            ImGui::PopID();
        }
        
        ImGui::End();
    }

    void ImGuiLayer::DrawSettings(ISceneObject* selected)
    {
        if(selected)
        {
            if(selected->GetType() == ObjectType::PointLight)
            {
                auto* light = dynamic_cast<PointLight*>(selected);
                
                ImGui::Text("PointLight Color");
                ImGui::ColorEdit3("Color", (float *)&light->GetColor(), ImGuiColorEditFlags_NoLabel);
                
                ImGui::Text("Ambient Intensity");
                ImGui::DragFloat3("##Ambient Intensity", (float *)&light->GetAmbientIntensity(), 0.01f, 0.0f, 1.0f, "%.3f");

                ImGui::Text("Diffuse Intensity");
                ImGui::DragFloat3("##Diffuse Intensity", (float *)&light->GetDiffuseIntensity(), 0.01f, 0.0f, 1.0f, "%.3f");

                ImGui::Text("Specular Intensity");
                ImGui::DragFloat3("##Specular Intensity", (float *)&light->GetSpecularIntensity(), 0.01f, 0.0f, 1.0f, "%.3f");
            }
            
            static std::string lastName;
            static char buffer[256] = {};
            
            if (selected && selected->GetName() != lastName)
            {
                memset(buffer, 0, sizeof(buffer));
                strncpy(buffer, selected->GetName().c_str(), sizeof(buffer) - 1);
                lastName = selected->GetName();
            }
            
            ImGui::Text("Name");
            // Now buffer persists across frames, and you don't lose changes!
            if(ImGui::InputText("##SelectedName", buffer, sizeof(buffer)))
            {
                selected->SetName(std::string(buffer));
                lastName = buffer;
            }
        }
    }

    void ImGuiLayer::LoadFont()
    {
        ImGuiIO& io = ImGui::GetIO();

        ImFontConfig cfg{};
        cfg.PixelSnapH = true;

        std::string path = GetProjectRootPath("/src/Resources/FiraCodeNerdFontMono-Regular.ttf");
                        
        
        static const ImWchar puaRanges[] = {
            0x0020, 0x00FF,   // keep your ASCII
            0xE000, 0xF8FF,   // all PUA (where Nerd-Font patches in icons)
            0
        };

        ImFont* nerd = io.Fonts->AddFontFromFileTTF(
            path.c_str(), 16.0f, &cfg, puaRanges
        );
        io.FontDefault = nerd;
    }

    void ImGuiLayer::DrawGizmos(Engine* engine,int gizmoOperation)
    {
        // check if an object is selected
        auto* selected = engine->GetSelectedObject();
        if(selected)
        {
            // Camera matrices
            glm::mat4 view = engine->GetCamera()->GetViewMatrix();
            glm::mat4 proj = engine->GetCamera()->GetProjectionMatrix();

            // the transform to manipulate
            glm::mat4 model = selected->GetModelMatrix();
            ImGuizmo::OPERATION operation = (ImGuizmo::OPERATION)gizmoOperation;
            static ImGuizmo::MODE mode = ImGuizmo::LOCAL;

            ImGui::Begin("MainDockSpaceHost");
            ImGuizmo::SetDrawlist();
            ImGuizmo::SetRect(ImGui::GetWindowPos().x,ImGui::GetWindowPos().y,
                ImGui::GetWindowWidth(), ImGui::GetWindowHeight());

            if(ImGuizmo::Manipulate(glm::value_ptr(view),glm::value_ptr(proj),
                operation,mode,glm::value_ptr(model)))
            {
                // apply new transform to the selected object
                selected->SetTransformFromMatrix(model);
            }
            ImGui::End();
        }
    }
}