#include "Engine.h"
#include <algorithm>
#include "Utility/Log.hpp"
#include "Mouse.h"
#include <utility>
#include "Graphics/TextureManager.h"
#include "Graphics/Tracer.h"

namespace isaacObjectViewer
{
    inline Mouse* MouseRef = Mouse::GetInstance();
    Engine *Engine::s_Instance = nullptr;

    Engine::Engine()
        : m_Window(nullptr),
          m_Shader(nullptr),
          m_MainShader(nullptr),
          m_Camera(nullptr),
          m_SelectedObject(nullptr),
          m_DirLight(nullptr),
          m_BlinnPhongShading(true),
          m_UseMaterial(true),
          m_MouseModeEnabled(true),
          m_FreeCameraModeEnabled(false),
          m_KeyPressed(false),
          m_IsRunning(false)
    {
        m_DirLight = new DirectionalLight();
    }

    void Engine::Run(bool fullscreen)
    {
        assert(Init("Isaac's 3D Object Loader", SCREEN_WIDTH, SCREEN_HEIGHT, fullscreen));
        
        m_ImGuiLayer.Init(m_Window->GetSDLWindow(),m_Window->GetGLContext());

        // Seed the frame timer and a sane first delta
        m_FrameTimer.Start();
        m_DeltaTime = 1.0f / 60.0f;
        m_FPS = 60.0f;

        while (m_IsRunning)
        {
            float deltaSeconds = m_FrameTimer.Stop();      
            m_FrameTimer.Start();                          

            if (deltaSeconds <= 0.0f) 
                deltaSeconds = 1.0f / 60.0f;

            m_DeltaTime = deltaSeconds;
            m_FPS = 1.0f / deltaSeconds;

            m_ImGuiLayer.Begin();
            m_ImGuiLayer.DrawUI();
            
            // input
            ProcessInput();
            Update(m_DeltaTime);
            Render();

            //  Manual FPS cap only when VSync is off
            if (!m_VSyncEnabled && m_FrameCapEnabled && m_FrameCapFps > 0)
            {
                const float target = 1.0f / float(m_FrameCapFps);
                float soFar = m_FrameTimer.Peek();                 
                if (soFar < target)
                {
                    const float ms = (target - soFar) * 1000.0f;
                    if (ms > 0.0f) SDL_Delay((Uint32)ms);
                }
            }

            m_ImGuiLayer.End();
            SDL_GL_SwapWindow(m_Window->GetSDLWindow());
        }
        Clean();
    }

    // @brief initializes the engine's dependencies, resources and objects
    bool Engine::Init(const char *title, int width, int height, bool fullscreen)
    {
        Log::Init();

        LOG_INFO("Welcome to Isaac's Graphics Engine!");

        m_Window = new Window(title,width,height,fullscreen);

        m_MouseModeEnabled = true;
        SDL_ShowCursor();
        SDL_SetWindowRelativeMouseMode(m_Window->GetSDLWindow(),false);

        // Initialize glad
        //----------------------------------------------------------------------------------------
        if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
        {
            LOG_ERROR("Failed to load OpenGL!");
            SDL_Quit();
        }
        //----------------------------------------------------------------------------------------

        glEnable(GL_DEPTH_TEST); // Enable depth testing
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        std::string colors_vs = GetProjectRootPath("src/Resources/Shaders/main.vs"); 
        std::string colors_fs = GetProjectRootPath("src/Resources/Shaders/main.fs"); 
        
        m_MainShader = new Shader(colors_vs.c_str(), colors_fs.c_str());
        m_MainShader->Bind();
                                        
        m_BackgroundColor = glm::vec3(0.0f, 0.0f, 0.0f);

        int display_w, display_h;
        SDL_GetWindowSizeInPixels(m_Window->GetSDLWindow(), &display_w, &display_h);

        m_Camera = new Camera(glm::vec3(-4.0f, 3.0f, 4.0f));
        m_Camera->LookAtTarget(glm::vec3(1.0f, 0.0f, 0.0f)); 
        m_Camera->SetProjection((float)display_w / (float)display_h);

        m_IsRunning = true;

        return true;
    }

    // @brief processes player input
    void Engine::ProcessInput()
    {   
        SDL_Event MainEvent;
        while (SDL_PollEvent(&MainEvent))
        {   
            // Process ImGui Events only if we are in mouse mode
            if(!m_FreeCameraModeEnabled)
                ImGui_ImplSDL3_ProcessEvent(&MainEvent);    
            
            // Note: its important to process imgui events before checking if mouse is over UI
            // to make sure the UI acn take input.
            if(m_ImGuiLayer.isMouseOverUI() || m_ImGuiLayer.isMouseOverGizmo())
            {
                continue; // Skip processing if mouse is over UI or gizmo
            }
            
            std::pair<float,float> mouseState;
            SDL_GetMouseState(&mouseState.first, &mouseState.second);

            if(MainEvent.type == SDL_EVENT_QUIT)
            {
                m_IsRunning = false;
            }

            if(MainEvent.type == SDL_EVENT_KEY_DOWN)
            {
                switch(MainEvent.key.key)
                {
                    case SDLK_ESCAPE:
                        EnableMouseMode();
                        break;
                    case SDLK_F4:
                        EnableFreeCameraMode();
                        break;
                    default:
                        break;       
                }
            }
            
            if(m_FreeCameraModeEnabled)
            {
                if(MainEvent.type == SDL_EVENT_MOUSE_MOTION)
                {
                    float xoffset = static_cast<float>(MainEvent.motion.xrel);
                    float yoffset = static_cast<float>(MainEvent.motion.yrel);

                    MouseRef->ProcessMotion(m_Camera, xoffset, -yoffset);
                }

                if(MainEvent.type == SDL_EVENT_MOUSE_WHEEL)
                {
                    float yoffset = static_cast<float>(MainEvent.wheel.y);
                    MouseRef->ProcessZoom(yoffset,m_Camera);

                     // Camera caches the projection matrix, so we refresh it here
                    int w, h; SDL_GetWindowSizeInPixels(m_Window->GetSDLWindow(), &w, &h);
                    m_Camera->SetProjection((float)w / (float)h);
                }
            }
            
            if(m_MouseModeEnabled)
            {
                if(MainEvent.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
                {
                    if(!m_ImGuiLayer.isMouseOverGizmo() && MainEvent.button.button == SDL_BUTTON_LEFT)
                    {
                        MouseRef->ProcessMouseClick(mouseState.first,mouseState.second, m_Camera);
                    }
                }
                if(MainEvent.type == SDL_EVENT_KEY_DOWN)
                {
                    switch(MainEvent.key.key)
                    {
                        case SDLK_T:
                            m_ImGuiLayer.SetGizmoOperation(GizmoMode::TRANSLATION);
                            break;
                        case SDLK_R:
                            m_ImGuiLayer.SetGizmoOperation(GizmoMode::ROTATION);
                            break;
                        case SDLK_S:
                            m_ImGuiLayer.SetGizmoOperation(GizmoMode::SCALE);
                            break;
                        case SDLK_H:
                            m_ImGuiLayer.SetGizmoOperation(GizmoMode::NONE);
                            break;
                        case SDLK_DELETE:
                            if(m_SelectedObject)
                            {
                                RemoveSceneObject(m_SelectedObject);
                                m_SelectedObject = nullptr;
                            }
                            break;
                        default:
                            break;    
                    }
                }    
            }
        }
    }

    // @brief updates all of the engine dependencies, resources and objects.
    void Engine::Update(float dt) 
    {
        if(!m_MouseModeEnabled)
            m_Camera->Update(dt);

        Tracer::GetInstance()->Update(dt);
    }

    // @brief renders all of the engine textures, sounds and objects.
    void Engine::Render()
    {
        int display_w, display_h;
        SDL_GetWindowSizeInPixels(m_Window->GetSDLWindow(), &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);

        glClearColor(m_BackgroundColor.x, m_BackgroundColor.y, m_BackgroundColor.z, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        m_MainShader->Bind();
        m_MainShader->setVec3("viewPos", m_Camera->GetPosition());
        
        SendAllLightsToShader();
        m_DirLight->SetUniforms(m_MainShader);
        m_MainShader->setBool("useBlinnPhong", m_BlinnPhongShading);

        glm::mat4 view = m_Camera->GetViewMatrix(); // VIEW
        glm::mat4 projection = m_Camera->GetProjectionMatrix(); 

        for (auto& obj : m_SceneObjects)
        {
            obj->Render(m_Renderer, view, projection, m_MainShader); 
        }

        Tracer::GetInstance()->Render(view, projection, display_w, display_h);
    }

    // @brief cleans all of the engine resources.
    void Engine::Clean()
    {
        TextureManager::UnloadAll();
        ClearSceneObjects();
        if(m_SelectedObject)
            delete m_SelectedObject;
        delete m_Camera;
        delete m_Shader;
        delete m_MainShader;
        delete m_Window;
        SDL_Quit();
    }

    // @brief loads up needed resources such as textures, sfx, music etc..
    bool Engine::LoadResources() { return false; } 

    void Engine::EnableMouseMode()
    {
        m_MouseModeEnabled = true;
        m_FreeCameraModeEnabled = false;
        SDL_ShowCursor();
        SDL_SetWindowRelativeMouseMode(m_Window->GetSDLWindow(),false);
    }
    
    void Engine::EnableFreeCameraMode()
    {
        m_MouseModeEnabled = false;
        m_FreeCameraModeEnabled = true;
        SDL_HideCursor();
        SDL_SetWindowRelativeMouseMode(m_Window->GetSDLWindow(),true);
    }
        
    void Engine::SendAllLightsToShader()
    {
        int numLights = std::min(int(m_LightObjects.size()),MAX_LIGHTS);

        for (int i = 0; i < numLights; ++i) 
        {
            m_MainShader->setVec3("point_lights[" + std::to_string(i) + "].position", m_LightObjects[i]->GetPosition());
            m_MainShader->setVec3("point_lights[" + std::to_string(i) + "].ambient", m_LightObjects[i]->GetAmbientIntensity());
            m_MainShader->setVec3("point_lights[" + std::to_string(i) + "].diffuse", m_LightObjects[i]->GetDiffuseIntensity());
            m_MainShader->setVec3("point_lights[" + std::to_string(i) + "].specular", m_LightObjects[i]->GetSpecularIntensity());
            m_MainShader->setFloat("point_lights["+ std::to_string(i) + "].constant", 1.0f);
            m_MainShader->setFloat("point_lights["+ std::to_string(i) + "].linear", 0.09f);
            m_MainShader->setFloat("point_lights["+ std::to_string(i) + "].quadratic", 0.032f);
        }
        m_MainShader->setInt("numPointLights", numLights);
        // ----------------------------------------------------
    }

} // namespace isaacGraphicsEngine
