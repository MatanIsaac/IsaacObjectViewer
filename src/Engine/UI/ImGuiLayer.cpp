#include "ImGuiLayer.h"
#include "../Core/Engine.h" // so we can access engine state like m_Camera, m_Light, etc.
#include "../Core/Mouse.h"
#include "TextureManager.h"
#include "ImGuiFileDialog/ImGuiFileDialog.h"
#include "Graphics/ModelManager.h"

namespace isaacObjectViewer
{
    ImGuiLayer::ImGuiLayer()
        : m_Window(nullptr)
        , m_GLContext(nullptr)
        , selected(nullptr)
        , m_GizmoOperation(ImGuizmo::TRANSLATE)
        , m_CurrentPath(std::filesystem::current_path().string())
        , m_SelectedPath("")
        , m_ImageDialogFilters("All Images{.png,.jpg,.jpeg},.png,.jpg,.jpeg")
        , m_ImportObjDialogFilters("All Objects{.obj,.fbx,.dae,},.obj,.fbx,.dae")
        , m_IsMouseOverUI(false)
        , M_RightPanelWidth(350.0f)
        , M_TopPanelHeight(40.0f)
    { }

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
        m_IsMouseOverUI = io.WantCaptureMouse;
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
            ImGui::DockBuilderSplitNode(
                dockspace_id, ImGuiDir_Right,
                M_RightPanelWidth / ImGui::GetMainViewport()->Size.x, 
                &right_id, &main_id);

            ImGuiID top_id, viewport_id;
            ImGui::DockBuilderSplitNode(
                main_id, ImGuiDir_Up,
                M_TopPanelHeight / ImGui::GetMainViewport()->Size.y,
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
                if (ImGui::MenuItem("Import 3D Object")) 
                { 
                    ImGuiFileDialog loadObjFileDialog;
                    IGFD::FileDialogConfig cfg;
                    cfg.flags = ImGuiFileDialogFlags_Modal;
                    cfg.path  = ".";
                    m_ImportObjectDialog.OpenDialog("Import 3D Object", "Select 3D Object", m_ImportObjDialogFilters, cfg);
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
        if (m_ImportObjectDialog.Display("Import 3D Object",32,{500.f,500.f}))
        {
            if (m_ImportObjectDialog.IsOk()) 
            {    
                std::string path = m_ImportObjectDialog.GetFilePathName();
                auto* model = ModelManager::GetInstance().LoadModel(path);
                if (model) 
                {
                    engine->AddSceneObject(model);
                    LOG_INFO("Size: {}", engine->GetSceneObjects().size());
                    engine->SetSelectedObject(model);
                }
                else
                {
                    LOG_ERROR("Failed to load model from path: {}", path);
                }
            } 
            else
            {
                LOG_ERROR("Model import cancelled or failed.");
            }
            m_ImportObjectDialog.Close();
        }

        ImGuiWindowClass window_class;
        window_class.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoTabBar;
        ImGui::SetNextWindowClass(&window_class);

        // No docking, collapse, or moving; and hide title bar for a clean look.
        ImGuiWindowFlags topPanelFlags = 
            ImGuiWindowFlags_NoMove | 
            ImGuiWindowFlags_NoDecoration;
        
        if (ImGui::Begin("Engine Controls", nullptr, topPanelFlags))
        {
            // Build a 3-column table: [ViewModes][CenterPlay][GizmoModes]
            if (ImGui::BeginTable("TopBarTable", 3,
                                ImGuiTableFlags_SizingStretchProp |
                                ImGuiTableFlags_BordersInnerV))
            {
                auto view_modes_width = ImGui::CalcTextSize("View Modes").x + 25.f;
                auto gizmo_modes_width = ImGui::CalcTextSize("Gizmo Modes").x 
                                       + ImGui::CalcTextSize("NONE").x 
                                       + ImGui::CalcTextSize("Translate").x 
                                       + ImGui::CalcTextSize("Rotate").x
                                       + ImGui::CalcTextSize("Scale").x + 120.f;
                ImGui::TableSetupColumn("##Blank", ImGuiTableColumnFlags_WidthStretch, 100.0f);
                ImGui::TableSetupColumn("ViewModes", ImGuiTableColumnFlags_WidthFixed, view_modes_width);
                ImGui::TableSetupColumn("GizmoModes", ImGuiTableColumnFlags_WidthFixed, gizmo_modes_width);

                ImGui::TableNextRow();

                ImGui::TableSetColumnIndex(0);
                {}                
                
                // ---------------- Left: View Modes ----------------
                ImGui::TableSetColumnIndex(1);
                {
                    const char* btn_label = "View Modes \uf0d7"; // ▼
                    if (ImGui::Button(btn_label))
                        ImGui::OpenPopup("modes_popup");

                    if (ImGui::BeginPopup("modes_popup"))
                    {
                        if (ImGui::MenuItem("Mouse mode"))       
                            engine->EnableMouseMode();
                        if (ImGui::MenuItem("Free camera mode")) 
                            engine->EnableFreeCameraMode();
                        ImGui::EndPopup();
                    }
                }
                // ---------------- Right: Gizmo Modes ----------------
                ImGui::TableSetColumnIndex(2);
                {
                    ImGui::TextUnformatted("Gizmo Mode:");
                    ImGui::SameLine();

                    // keep them compact
                    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, ImGui::GetStyle().ItemSpacing.y));
                    ImGui::RadioButton("NONE",      &m_GizmoOperation, ImGuizmo::NONE);      ImGui::SameLine();
                    ImGui::RadioButton("Translate", &m_GizmoOperation, ImGuizmo::TRANSLATE); ImGui::SameLine();
                    ImGui::RadioButton("Rotate",    &m_GizmoOperation, ImGuizmo::ROTATE);    ImGui::SameLine();
                    ImGui::RadioButton("Scale",     &m_GizmoOperation, ImGuizmo::SCALE);
                    ImGui::PopStyleVar();
                }
                ImGui::EndTable();
            }
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

                // Draw Calls, triangles, vertices
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); 
                ImGui::TextUnformatted("Draw Calls");
                auto frameStats = engine->GetFrameStats();
                ImGui::TableSetColumnIndex(1);
                ImGui::SetNextItemWidth(-FLT_MIN);
                ImGui::Text("%u", frameStats.DrawCalls);
                
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); 
                ImGui::TextUnformatted("Triangles");
                ImGui::TableSetColumnIndex(1);
                ImGui::SetNextItemWidth(-FLT_MIN);
                ImGui::Text("%lu", frameStats.Triangles);
                
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); 
                ImGui::TextUnformatted("Vertices");
                ImGui::TableSetColumnIndex(1);
                ImGui::SetNextItemWidth(-FLT_MIN);
                ImGui::Text("%lu", frameStats.Vertices);

                // Frametime
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); 
                ImGui::TextUnformatted("Frame Time");
                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%.4f ms", engine->GetFrameTime() * 1000.0f);

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
                if (ImGui::Checkbox("##VSync", &engine->IsVSyncEnabled()))
                {
                    engine->ApplySwapInterval();
                }

                // Cap FPS (only if VSync is disabled)
                if (!engine->IsVSyncEnabled())
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

                /* 
                 *  TODO: Add the following
                 *  1. Frame time (ms) (primary) and FPS (secondary).
                 *  2. Add Draw Calls, Batches, and Instance Rendering
                 *  3. Triangles, Vertices, Indices
                */
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

                // Wireframe Mode
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); 
                ImGui::TextUnformatted("Wireframe Mode");
                ImGui::TableSetColumnIndex(1);
                ImGui::SetNextItemWidth(-FLT_MIN);
                if (ImGui::Checkbox("##WireframeMode", &engine->IsWireframeModeEnabled()))
                {
                    engine->ToggleWireframeMode();
                }

                /*
                 *  TODO:
                 *  1. Add Anti-Aliasing
                */

                // Shading model: Unlit / Phong / Blinn-Phong
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::TextUnformatted("Shading");
                ImGui::TableSetColumnIndex(1);
                ImGui::SetNextItemWidth(-FLT_MIN);
                const char* shadingModes[] = { "Unlit", "Phong", "Blinn-Phong" };
                int shadingMode = engine->GetShadingMode(); // 0=unlit, 1=Phong, 2=Blinn-Phong
                if (ImGui::Combo("##Shading", &shadingMode, shadingModes, IM_ARRAYSIZE(shadingModes)))
                {
                    engine->SetShading(static_cast<ShadingType>(shadingMode));
                }

                ImGui::EndTable();
            }

            if (ImGui::CollapsingHeader("Directional Light")) 
            {
                auto& dirLight = engine->GetDirectionalLight();

                // Persist UI state across frames
                static bool init = true;
                static glm::vec3 baseAmbient, baseDiffuse, baseSpecular;
                static float ambient_mult  = 1.0f;
                static float diffuse_mult  = 1.0f;
                static float specular_mult = 1.0f;

                if (init) 
                {
                    baseAmbient  = dirLight->GetAmbient(); 
                    baseDiffuse  = dirLight->GetDiffuse();
                    baseSpecular = dirLight->GetSpecular();
                    init = false;
                }

                glm::vec3 direction = dirLight->GetDirection();

                bool changed = false;
                ImGui::PushID("DirLight");

                if (ImGui::BeginTable("DirLightTable", 2, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingStretchProp))
                {
                    ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, 140.0f);
                    ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);
                    ImGui::SetNextItemWidth(-FLT_MIN);

                    bool enabled = dirLight->IsEnabled();
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0); ImGui::Text("Enabled");
                    ImGui::TableSetColumnIndex(1);
                    ImGui::SetNextItemWidth(-FLT_MIN);
                    if (ImGui::Checkbox("##Enabled", &enabled))
                        dirLight->SetEnabled(enabled);

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
                    dirLight->SetDirection(direction);
                    dirLight->SetAmbient (baseAmbient  * ambient_mult);
                    dirLight->SetDiffuse (baseDiffuse  * diffuse_mult);
                    dirLight->SetSpecular(baseSpecular * specular_mult);
                }
            }


        }
        
        ImGui::Separator();

        if (ImGui::CollapsingHeader("Camera Settings"))
        {
            std::unique_ptr<Camera>& cam = engine->GetCamera();

            // snapshot current values
            glm::vec3 pos = cam->GetPosition();
            float speed   = cam->GetSpeed();
            float near_plane = cam->GetNear();
            float far_plane = cam->GetFar();

            bool changed_pos   = false;
            bool changed_speed = false;
            bool changed_fov   = false;
            bool changed_near  = false;
            bool changed_far   = false;

            int display_w, display_h;
            SDL_GetWindowSizeInPixels(engine->GetSDLWindow(), &display_w, &display_h);

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

                // FOV
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::TextUnformatted("FOV");
                ImGui::TableSetColumnIndex(1);
                ImGui::SetNextItemWidth(-FLT_MIN);
                changed_fov |= ImGui::DragFloat("##cam_fov", &cam->GetFOV(), 0.1f, 1.0f, 90.0f, "%.2f");
                ImGui::SameLine();
                if (ImGui::SmallButton("Reset##fov"))
                {
                    cam->SetFOV(Camera::DEFAULT_CAMERA_FOV);
                    changed_fov = true;
                }

                // Near Plane
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::TextUnformatted("Near Plane");
                ImGui::TableSetColumnIndex(1);
                ImGui::SetNextItemWidth(-FLT_MIN);
                changed_near |= ImGui::DragFloat("##cam_near", &near_plane, 0.1f, 0.1f, 100.0f, "%.2f");
                ImGui::SameLine();
                if (ImGui::SmallButton("Reset##near"))
                {
                    cam->SetNear(Camera::DEFAULT_CAMERA_NEAR);
                    changed_near = true;
                }

                // Far Plane
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::TextUnformatted("Far Plane");
                ImGui::TableSetColumnIndex(1);
                ImGui::SetNextItemWidth(-FLT_MIN);
                changed_far |= ImGui::DragFloat("##cam_far", &far_plane, 0.1f, 0.1f, 100'000.0f, "%.2f");
                ImGui::SameLine();
                if (ImGui::SmallButton("Reset##far"))
                {
                    cam->SetFar(Camera::DEFAULT_CAMERA_FAR);
                    changed_far = true;
                }

                ImGui::EndTable();
            }

            // apply only if changed
            if (changed_pos)   
            {
                cam->SetPosition(pos);
            }
            if (changed_speed) 
            {
                cam->SetSpeed(speed);
            }
            if (changed_fov)
            {
                cam->SetFOV(cam->GetFOV());
                cam->SetProjection((float)display_w / (float)display_h);
            }
            if(changed_near) 
            {
                cam->SetNear(near_plane);
                cam->SetProjection((float)display_w / (float)display_h);
            }
            if(changed_far)
            {
                cam->SetFar(far_plane);
                cam->SetProjection((float)display_w / (float)display_h);
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
                    
        auto& sceneObjects = engine->GetSceneObjects();
        selected = engine->GetSelectedObject();

        for (size_t i = 0; i < sceneObjects.size(); ++i) 
        {
            IObject* obj = sceneObjects[i].get();
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

    void ImGuiLayer::DrawSettings(IObject* selected)
    {
        static std::string lastName;
        static char buffer[256] = {};
        
        if (selected && selected->GetName() != lastName)
        {
            memset(buffer, 0, sizeof(buffer));
            strncpy(buffer, selected->GetName().c_str(), sizeof(buffer) - 1);
            lastName = selected->GetName();
        }

        ImGui::SeparatorText("Transform");
        if (ImGui::BeginTable("TransformTable", 2, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingStretchProp)) 
        {                
            ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, 120.0f);
            ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);

            // Position
            glm::vec3 pos = selected->GetPosition();
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0); ImGui::Text("Position");
            ImGui::TableSetColumnIndex(1);
            ImGui::SetNextItemWidth(-FLT_MIN);
            if (ImGui::DragFloat3("##Position", &pos.x, 0.01f))
                selected->SetPosition(pos);

            // Rotation (Euler in degrees)
            glm::vec3 rot = selected->GetRotation();
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0); ImGui::Text("Rotation");
            ImGui::TableSetColumnIndex(1);
            ImGui::SetNextItemWidth(-FLT_MIN);
            if (ImGui::DragFloat3("##Rotation", &rot.x, 0.01f))
            {
                selected->SetRotation(rot);
                selected->SetOrientation(glm::quat(glm::radians(rot)));
            }

            // Scale
            glm::vec3 scl = selected->GetScale();
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0); ImGui::Text("Scale");
            ImGui::TableSetColumnIndex(1);
            ImGui::SetNextItemWidth(-FLT_MIN);
            if (ImGui::DragFloat3("##Scale", &scl.x, 0.01f))
                selected->SetScale(scl);

            ImGui::EndTable();
        }

        if (ImGui::BeginTable("NameTable", 2, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingStretchProp))
        {
            ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, 120.0f);
            ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);

            // Name
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0); ImGui::Text("Name");
            ImGui::TableSetColumnIndex(1);
            ImGui::SetNextItemWidth(-FLT_MIN);
            if (ImGui::InputText("##Name", buffer, sizeof(buffer), ImGuiInputTextFlags_EnterReturnsTrue))
            {
                selected->SetName(std::string(buffer));
                lastName = selected->GetName();
            }

            ImGui::EndTable();
        }

        if (selected->GetType() == ObjectType::PointLight)
        {
            auto* light = dynamic_cast<PointLight*>(selected);

            ImGui::SeparatorText("Point Light Settings");
            if (ImGui::BeginTable("PointLightTable", 2, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingStretchProp))
            {
                ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, 120.0f);
                ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);

                bool enabled = light->IsEnabled();
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::Text("Enabled");
                ImGui::TableSetColumnIndex(1);
                ImGui::SetNextItemWidth(-FLT_MIN);
                if (ImGui::Checkbox("##Enabled", &enabled))
                    light->SetEnabled(enabled);

                glm::vec3 color = light->GetColor();
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::Text("Color");
                ImGui::TableSetColumnIndex(1);
                ImGui::SetNextItemWidth(-FLT_MIN);
                if (ImGui::ColorEdit3("Color", (float *)&color, ImGuiColorEditFlags_NoLabel))
                    light->SetColor(color);

                glm::vec3 ambient = light->GetAmbientIntensity();
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::Text("Ambient");
                ImGui::TableSetColumnIndex(1);
                ImGui::SetNextItemWidth(-FLT_MIN);
                if (ImGui::DragFloat3("##Ambient", (float *)&ambient, 0.01f, 0.0f, 1.0f, "%.3f"))
                    light->SetAmbientIntensity(ambient);

                glm::vec3 diffuse = light->GetDiffuseIntensity();
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::Text("Diffuse");
                ImGui::TableSetColumnIndex(1);
                ImGui::SetNextItemWidth(-FLT_MIN);
                if (ImGui::DragFloat3("##Diffuse", (float *)&diffuse, 0.01f, 0.0f, 1.0f, "%.3f"))
                    light->SetDiffuseIntensity(diffuse);

                glm::vec3 specular = light->GetSpecularIntensity();
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::Text("Specular");
                ImGui::TableSetColumnIndex(1);
                ImGui::SetNextItemWidth(-FLT_MIN);
                if (ImGui::DragFloat3("##Specular", (float *)&specular, 0.01f, 0.0f, 1.0f, "%.3f"))
                    light->SetSpecularIntensity(specular);

                ImGui::EndTable();
            }

            ImGui::SeparatorText("Attenuation");
            if (ImGui::BeginTable("AttenuationTable", 2, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingStretchProp))
            {
                ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, 120.0f);
                ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);

                // Position
                float attConstant = light->GetAttConstant();
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::Text("Constant");
                ImGui::TableSetColumnIndex(1);
                ImGui::SetNextItemWidth(-FLT_MIN);
                if (ImGui::DragFloat("##Attenuation Constant", &attConstant, 0.01f, 0.0f, 1.0f, "%.3f"))
                    light->SetAttConstant(attConstant);

                float attLinear = light->GetAttLinear();
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::Text("Linear");
                ImGui::TableSetColumnIndex(1);
                ImGui::SetNextItemWidth(-FLT_MIN);
                if (ImGui::DragFloat("##Attenuation Linear", &attLinear, 0.01f, 0.0f, 1.0f, "%.3f"))
                    light->SetAttLinear(attLinear);

                float attQuadratic = light->GetAttQuadratic();
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::Text("Quadratic");
                ImGui::TableSetColumnIndex(1);
                ImGui::SetNextItemWidth(-FLT_MIN);
                if (ImGui::DragFloat("##Attenuation Quadratic", &attQuadratic, 0.01f, 0.0f, 1.0f, "%.3f"))
                    light->SetAttQuadratic(attQuadratic);

                ImGui::EndTable();
            }
        }
        
        if (selected->GetType() == ObjectType::Cube  ||
            selected->GetType() == ObjectType::Plane || 
            selected->GetType() == ObjectType::Imported)
        {
            ImGui::SeparatorText("Material Settings");
        
            bool useMat = selected->GetUseMaterial();
            if (ImGui::Button(useMat ? "Use Object Color" : "Use Material"))
                selected->SetUseMaterial(!useMat);

            // Show color picker when not using material
            if (!selected->GetUseMaterial())
            {
                glm::vec3 color = selected->GetColor();
                if (ImGui::ColorEdit3("Object Color", &color.x))
                    selected->SetColor(color);
            }

            const char* filter_label = "Filter Mode \uf0d7"; // ▼

            if (ImGui::Button(filter_label))
                ImGui::OpenPopup("filter_popup");

            if (ImGui::BeginPopup("filter_popup"))
            {
                if (ImGui::MenuItem("Linear mode"))       
                {
                    LOG_INFO("Switched to Linear mode");
                    selected->SetFilterMode(TextureFilterMode::LINEAR);
                }
                if (ImGui::MenuItem("Nearest mode")) 
                {
                    LOG_INFO("Switched to Nearest mode");
                    selected->SetFilterMode(TextureFilterMode::NEAREST);
                }
                ImGui::EndPopup();
            }

            float shininess = selected->GetShininess();
            ImGui::Text("Shininess");
            if (ImGui::DragFloat("##Shininess", &shininess, 0.01f, 0.0f, 100.0f, "%.3f"))
                selected->SetShininess(shininess);

            if (ImGui::Button("Load Diffuse Texture"))
            {
                IGFD::FileDialogConfig cfg;
                cfg.flags = ImGuiFileDialogFlags_Modal;
                cfg.path  = ".";
                m_DiffuseFileDialog.OpenDialog("LoadDiffuse", "Select Diffuse", m_ImageDialogFilters, cfg);
            }
            if(selected->GetMaterial().Diffuse)
            {
                std::string text = GetFileName(selected->GetMaterial().Diffuse->GetPath());
                ImGui::Text("Diffuse Texture: ");
                ImGui::SameLine();
                char* buf = new char[text.size() + 1];
                std::strcpy(buf, text.c_str());
                ImGui::InputText("##DiffuseTexture", buf, text.size() + 1, ImGuiInputTextFlags_ReadOnly);
                delete[] buf;
            }

            if (ImGui::Button("Load Normal Texture"))
            {
                IGFD::FileDialogConfig cfg;
                cfg.flags = ImGuiFileDialogFlags_Modal;
                cfg.path  = ".";
                m_NormalFileDialog.OpenDialog("LoadNormal", "Select Normal", m_ImageDialogFilters, cfg);
            }
            if(selected->GetMaterial().Normal)
            {
                std::string text = GetFileName(selected->GetMaterial().Normal->GetPath());
                ImGui::Text("Normal Texture: ");
                ImGui::SameLine();
                char* buf = new char[text.size() + 1];
                std::strcpy(buf, text.c_str());
                ImGui::InputText("##NormalTexture", buf, text.size() + 1, ImGuiInputTextFlags_ReadOnly);
                delete[] buf;
            }

            if (ImGui::Button("Load Specular Texture"))
            {
                IGFD::FileDialogConfig cfg;
                cfg.flags = ImGuiFileDialogFlags_Modal;
                cfg.path  = ".";
                m_SpecularFileDialog.OpenDialog("LoadSpecular", "Select Specular", m_ImageDialogFilters, cfg);
            }
            
            if(selected->GetMaterial().Specular)
            {
                std::string text = GetFileName(selected->GetMaterial().Specular->GetPath());
                ImGui::Text("Specular Texture: ");
                ImGui::SameLine();
                char* buf = new char[text.size() + 1];
                std::strcpy(buf, text.c_str());
                ImGui::InputText("##SpecularTexture", buf, text.size() + 1, ImGuiInputTextFlags_ReadOnly);
                delete[] buf;
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
                        auto material = selected->GetMaterial();
                        material.Diffuse = tex;
                        selected->SetMaterial(material);
                    }
                }
                m_DiffuseFileDialog.Close();
            }

            if (m_NormalFileDialog.Display("LoadNormal",32,{100.f,100.f}))
            {
                if (m_NormalFileDialog.IsOk())
                {
                    std::string path = m_NormalFileDialog.GetFilePathName();
                    auto tex = TextureManager::LoadTexture(path,TextureType::NORMAL);

                    auto type = selected->GetType();
                    if (type == ObjectType::Cube || type == ObjectType::Plane || type == ObjectType::Imported)
                    {
                        auto material = selected->GetMaterial();
                        material.Normal = tex;
                        selected->SetMaterial(material);
                    }
                }
                m_NormalFileDialog.Close();
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
                        auto material = selected->GetMaterial();
                        material.Specular = tex;
                        selected->SetMaterial(material);
                    }
                }
                m_SpecularFileDialog.Close();
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
        if(!engine)
        {
            LOG_ERROR("ImGuiLayer::DrawGizmos: Engine instance is null.");
            return;
        }

        if(engine->IsFreeCameraModeEnabled())
        {
            return;
        }

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
    void ImGuiLayer::DrawBlankTableRow()
    {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::TableSetColumnIndex(1);
        ImGui::SetNextItemWidth(-FLT_MIN);
    }
}