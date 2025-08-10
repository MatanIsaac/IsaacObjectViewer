#include "ImGuiLayer.h"
#include "../Core/Engine.h" // so we can access engine state like m_Camera, m_Light, etc.
#include "../Core/Mouse.h"
#include "TextureManager.h"
#include "ImGuiFileDialog/ImGuiFileDialog.h"
#include "Graphics/ModelManager.h"

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
                if (ImGui::MenuItem("Import Obj")) 
                { 
                    ImGuiFileDialog loadObjFileDialog;
                    IGFD::FileDialogConfig cfg;
                    cfg.flags = ImGuiFileDialogFlags_Modal;
                    cfg.path  = ".";
                    m_ImportObjDlg.OpenDialog("ImportObj", "Select Obj", ".obj", cfg);
                }
                ImGui::Separator();
                if (ImGui::MenuItem("Exit")) 
                { 
                    engine->Exit();
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
        if (m_ImportObjDlg.Display("ImportObj",32,{100.f,100.f}))
        {
            if (m_ImportObjDlg.IsOk())
            {
                std::string path = m_ImportObjDlg.GetFilePathName();        
                ModelManager::LoadObjModel(path);
            }
            m_ImportObjDlg.Close();
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
        ImGui::RadioButton("NONE", &m_GizmoOperation, ImGuizmo::NONE); ImGui::SameLine();
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

        if (ImGui::CollapsingHeader("Performance"))
        {
            if (ImGui::BeginTable("PerformanceTable", 2, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingStretchProp))
            {
                ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, 140.0f);
                ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);

                // FPS
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); 
                ImGui::TextUnformatted("FPS");
                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%.4f", engine->GetFPS());

                // VSync
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); 
                ImGui::TextUnformatted("VSync");
                ImGui::TableSetColumnIndex(1);
                if (ImGui::Checkbox("##VSync", &engine->IsVSyncEanbled()))
                {
                    engine->ApplySwapInterval();
                }

                // Cap FPS (only if VSync is disabled)
                if (!engine->IsVSyncEanbled())
                {
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0); 
                    ImGui::TextUnformatted("Cap FPS");
                    ImGui::TableSetColumnIndex(1);
                    ImGui::Checkbox("##CapFPS", &engine->GetFrameCapEnabled());

                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0); 
                    ImGui::TextUnformatted("Cap Value");
                    ImGui::TableSetColumnIndex(1);
                    ImGui::SetNextItemWidth(-FLT_MIN);
                    ImGui::DragInt("##CapValue", &engine->GetFrameCapFps(), 1, 30, 240);
                }

                ImGui::EndTable();
            }
        }


        if (ImGui::CollapsingHeader("Environment Settings"))
        {
            if (ImGui::BeginTable("EnvironmentTable", 2, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingStretchProp))
            {
                ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, 140.0f);
                ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);

                // Background Color row
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::TextUnformatted("Background Color");

                ImGui::TableSetColumnIndex(1);
                ImGui::SetNextItemWidth(-FLT_MIN);
                ImGui::ColorEdit3("##BackgroundColor", (float*)&engine->GetBackgroundColor(),ImGuiColorEditFlags_NoLabel);
                ImGui::SameLine();
                if (ImGui::SmallButton("Reset##BackgroundColor"))
                {
                    engine->SetBackgroundColor({0.0f, 0.0f, 0.0f});
                }

                ImGui::EndTable();
            }

            if (ImGui::CollapsingHeader("Directional Light")) 
            {
                DirectionalLight& dirLight = engine->GetDirectionalLight();

                // Persist UI state across frames
                static bool init = true;
                static glm::vec3 baseAmbient, baseDiffuse, baseSpecular;
                static float ambient_mult  = 1.0f;
                static float diffuse_mult  = 1.0f;
                static float specular_mult = 1.0f;

                if (init) 
                {
                    baseAmbient  = dirLight.GetAmbient(); 
                    baseDiffuse  = dirLight.GetDiffuse();
                    baseSpecular = dirLight.GetSpecular();
                    init = false;
                }

                glm::vec3 direction = dirLight.GetDirection();

                bool changed = false;
                ImGui::PushID("DirLight");

                if (ImGui::BeginTable("DirLightTable", 2, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingStretchProp))
                {
                    ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, 140.0f);
                    ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);
                    ImGui::SetNextItemWidth(-FLT_MIN);

                    // Direction
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0); ImGui::Text("Direction");
                    ImGui::SetNextItemWidth(-FLT_MIN);
                    ImGui::TableSetColumnIndex(1); changed |= ImGui::DragFloat3("##Direction_", &direction.x, 0.01f);

                    // Ambient
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0); ImGui::Text("Ambient");
                    ImGui::SetNextItemWidth(-FLT_MIN);
                    ImGui::TableSetColumnIndex(1); changed |= ImGui::DragFloat3("##Ambient_", &baseAmbient.x, 0.01f, 0.0f, 1.0f);

                    // Ambient Mult
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0); ImGui::Text("Ambient Mult");
                    ImGui::SetNextItemWidth(-FLT_MIN);
                    ImGui::TableSetColumnIndex(1); changed |= ImGui::DragFloat("##AmbientMult_", &ambient_mult, 0.05f, 0.0f, 5.0f, "%.2f");

                    // Diffuse
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0); ImGui::Text("Diffuse");
                    ImGui::SetNextItemWidth(-FLT_MIN);
                    ImGui::TableSetColumnIndex(1); changed |= ImGui::DragFloat3("##Diffuse_", &baseDiffuse.x, 0.01f, 0.0f, 1.0f);

                    // Diffuse Mult
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0); ImGui::Text("Diffuse Mult");
                    ImGui::SetNextItemWidth(-FLT_MIN);
                    ImGui::TableSetColumnIndex(1); changed |= ImGui::DragFloat("##DiffuseMult_", &diffuse_mult, 0.05f, 0.0f, 5.0f, "%.2f");

                    // Specular
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0); ImGui::Text("Specular");
                    ImGui::SetNextItemWidth(-FLT_MIN);
                    ImGui::TableSetColumnIndex(1); changed |= ImGui::DragFloat3("##Specular_", &baseSpecular.x, 0.01f, 0.0f, 1.0f);

                    // Specular Mult
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0); ImGui::Text("Specular Mult");
                    ImGui::SetNextItemWidth(-FLT_MIN);
                    ImGui::TableSetColumnIndex(1); changed |= ImGui::DragFloat("##SpecularMult_", &specular_mult, 0.05f, 0.0f, 5.0f, "%.2f");
                    
                    ImGui::EndTable();
                }

                ImGui::PopID();

                if (changed) 
                {
                    dirLight.SetDirection(direction);
                    dirLight.SetAmbient (baseAmbient  * ambient_mult);
                    dirLight.SetDiffuse (baseDiffuse  * diffuse_mult);
                    dirLight.SetSpecular(baseSpecular * specular_mult);
                }
            }


        }
        
        ImGui::Separator();

        if (ImGui::CollapsingHeader("Camera Settings"))
        {
            Camera* cam = engine->GetCamera();

            // snapshot current values
            glm::vec3 pos = cam->GetPosition();
            float speed   = cam->GetSpeed();
            float zoom    = cam->GetZoom();

            bool changed_pos   = false;
            bool changed_speed = false;
            bool changed_zoom  = false;

            if (ImGui::BeginTable("CameraTable", 2, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingStretchProp))
            {
                ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, 140.0f);
                ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);

                // Position
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::TextUnformatted("Position");
                ImGui::TableSetColumnIndex(1);
                ImGui::SetNextItemWidth(-FLT_MIN);
                changed_pos |= ImGui::DragFloat3("##cam_pos", &pos.x, 0.01f);
                ImGui::SameLine();
                if (ImGui::SmallButton("Reset##pos"))
                {
                    pos = glm::vec3(0.0f, 0.0f, 0.0f);
                    changed_pos = true;
                }

                // Speed
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::TextUnformatted("Movement Speed");
                ImGui::TableSetColumnIndex(1);
                ImGui::SetNextItemWidth(-FLT_MIN);
                changed_speed |= ImGui::DragFloat("##cam_speed", &speed, 0.01f, 0.1f, 20.0f, "%.2f");
                ImGui::SameLine();
                if (ImGui::SmallButton("Reset##speed"))
                {
                    speed = Camera::DEFAULT_CAMERA_SPEED;
                    changed_speed = true;
                }

                // Zoom (FOV)
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::TextUnformatted("Zoom (FOV)");
                ImGui::TableSetColumnIndex(1);
                ImGui::SetNextItemWidth(-FLT_MIN);
                changed_zoom |= ImGui::DragFloat("##cam_zoom", &zoom, 0.1f, 1.0f, 90.0f, "%.2f");
                ImGui::SameLine();
                if (ImGui::SmallButton("Reset##zoom"))
                {
                    zoom = Camera::DEFAULT_CAMERA_ZOOM;
                    changed_zoom = true;
                }

                ImGui::EndTable();
            }

            // apply only if changed
            if (changed_pos)   cam->SetPosition(pos);
            if (changed_speed) cam->SetSpeed(speed);
            if (changed_zoom)
            {
                cam->SetZoom(zoom);
                int display_w, display_h;
                SDL_GetWindowSizeInPixels(engine->GetSDLWindow(), &display_w, &display_h);
                cam->SetProjection((float)display_w / (float)display_h, 0.1f, 100.0f);
            }
        }

        
        ImGui::Separator();
        
        if (ImGui::CollapsingHeader("Mouse Settings"))
        {
            Mouse* mouse = Mouse::GetInstance();

            // snapshot
            float sensitivity = mouse->GetSensitivity();
            bool changed = false;

            if (ImGui::BeginTable("MouseTable", 2, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingStretchProp))
            {
                ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, 140.0f);
                ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);

                // Sensitivity
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); 
                ImGui::TextUnformatted("Mouse Sensitivity");

                ImGui::TableSetColumnIndex(1);
                ImGui::SetNextItemWidth(-FLT_MIN);
                changed |= ImGui::DragFloat("##mouse_sens", &sensitivity, 0.01f, 0.01f, 5.0f, "%.4f");
                ImGui::SameLine();
                if (ImGui::SmallButton("Reset##mouse_sens"))
                {
                    sensitivity = Mouse::DEFAULT_SENSITIVITY;
                    changed = true;
                }

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::TextUnformatted("Trace Mouse Ray");
                ImGui::TableSetColumnIndex(1);
                ImGui::SetNextItemWidth(-FLT_MIN);
                changed |= ImGui::Checkbox("##trace_mouse_ray", &mouse->GetTraceMouseRay());
                ImGui::SameLine();
                if (ImGui::SmallButton("Reset##trace_mouse_ray"))
                {
                    mouse->GetTraceMouseRay() = false;
                    changed = true;
                }

                ImGui::EndTable();
            }

            // apply only if changed
            if (changed)
                mouse->SetSensitivity(sensitivity);
        }

        ImGui::Separator();
        
        if(selected)
        {
            if (ImGui::CollapsingHeader("Settings",ImGuiTreeNodeFlags_DefaultOpen)) // ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Leaf
            {    
                DrawSettings(selected);
            }
            ImGui::Separator();
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

        if (ImGui::CollapsingHeader("Transform",ImGuiTreeNodeFlags_DefaultOpen))
        {
            if (ImGui::BeginTable("TransformTable", 2, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingStretchProp)) 
            {                
                ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, 120.0f);
                ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::Text("Position");
                ImGui::SetNextItemWidth(-FLT_MIN);
                ImGui::TableSetColumnIndex(1); ImGui::DragFloat3("##Position", (float *)&selected->GetPosition(), 0.01f);
                
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::Text("Rotation");
                ImGui::SetNextItemWidth(-FLT_MIN);
                ImGui::TableSetColumnIndex(1); 
                if(ImGui::DragFloat3("##Rotation", (float *)&selected->GetRotation(), 0.01f))
                {
                    selected->SetOrientation(glm::quat(glm::radians(selected->GetRotation())));
                } 
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::Text("Scale");
                ImGui::SetNextItemWidth(-FLT_MIN);
                ImGui::TableSetColumnIndex(1); ImGui::DragFloat3("##Scale", (float *)&selected->GetScale(), 0.01f);
                ImGui::EndTable();
            }
        }

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
        
        if (selected->GetType() == ObjectType::Cube ||
        selected->GetType() == ObjectType::Plane || selected->GetType() == ObjectType::Imported)
        {
            ImGui::Separator();
            if (ImGui::CollapsingHeader("Material Settings"))    
            {    
                ImGui::Text("Shininess");
                ImGui::DragFloat("##Shininess", &selected->GetShininess(), 0.01f, 0.0f, 100.0f, "%.3f");
                
                if (ImGui::Button("Load Diffuse Texture"))
                {
                    IGFD::FileDialogConfig cfg;
                    cfg.flags = ImGuiFileDialogFlags_Modal;
                    cfg.path  = ".";
                    m_DiffuseFileDialog.OpenDialog("LoadDiffuse", "Select Diffuse", m_ImageDialogFilters, cfg);
                }
                
                if (ImGui::Button("Load Specular Texture"))
                {
                    IGFD::FileDialogConfig cfg;
                    cfg.flags = ImGuiFileDialogFlags_Modal;
                    cfg.path  = ".";
                    m_SpecularFileDialog.OpenDialog("LoadSpecular", "Select Specular", m_ImageDialogFilters, cfg);
                }
                
                if (m_DiffuseFileDialog.Display("LoadDiffuse",32,{100.f,100.f}))
                {
                    if (m_DiffuseFileDialog.IsOk())
                    {
                        std::string path = m_DiffuseFileDialog.GetFilePathName();
                        auto tex = TextureManager::LoadTexture(path,TextureType::DIFFUSE);
                        
                        auto type = selected->GetType();
                        if (type == ObjectType::Cube || type == ObjectType::Plane || type == ObjectType::Imported)
                        {
                            selected->SetDiffuseTexture(tex);    
                        }
                    }
                    m_DiffuseFileDialog.Close();
                }
                
                if (m_SpecularFileDialog.Display("LoadSpecular",32,{100.f,100.f}))
                {
                    if (m_SpecularFileDialog.IsOk())
                    {
                        std::string path = m_SpecularFileDialog.GetFilePathName();
                        auto tex = TextureManager::LoadTexture(path,TextureType::SPECULAR);
                        
                        auto type = selected->GetType();
                        if (type == ObjectType::Cube || type == ObjectType::Plane || type == ObjectType::Imported)
                        {
                            selected->SetSpecularTexture(tex);    
                        }
                    }
                    m_SpecularFileDialog.Close();
                }
                ImGui::Separator();
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
            path.c_str(), 14.0f, &cfg, puaRanges
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
        else
        {
            static bool first_time = false;
            if (first_time)
            {
                first_time = !first_time;
                SetGizmoOperation(GizmoMode::NONE);
            }
        }
    }
}