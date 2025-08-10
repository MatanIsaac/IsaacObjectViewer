#include "Engine.h"
#include <algorithm>
#include "Utility/Log.hpp"
#include "Mouse.h"
#include <utility>
#include "Graphics/TextureManager.h"
#include "Graphics/Primitives/Tracer.h"

namespace isaacObjectViewer
{
    inline Mouse* MouseRef = Mouse::GetInstance();
    Engine *Engine::s_Instance = nullptr;

    Engine::Engine()
        : m_Window(nullptr),
          m_Shader(nullptr),
          m_lightingShader(nullptr),
          m_Camera(nullptr),
          m_SelectedObject(nullptr),
          m_DirLight(nullptr),
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
        
        m_lightingShader = new Shader(colors_vs.c_str(), colors_fs.c_str());
        m_lightingShader->Bind();
                                        
        m_BackgroundColor = glm::vec3(0.35f, 0.35f, 0.35f);

        int display_w, display_h;
        SDL_GetWindowSizeInPixels(m_Window->GetSDLWindow(), &display_w, &display_h);

        m_Camera = new Camera(glm::vec3(0.0f, 2.0f, 5.0f));
        m_Camera->SetProjection((float)display_w / (float)display_h, 0.1f,100.0f);
        
        
        m_IsRunning = true;

        return true;
    }

    // @brief processes player input
    void Engine::ProcessInput()
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if(!m_FreeCameraModeEnabled)
                ImGui_ImplSDL3_ProcessEvent(&event);

            std::pair<float,float> mouseState;
            SDL_GetMouseState(&mouseState.first, &mouseState.second);

            if(event.type == SDL_EVENT_QUIT)
            {
                m_IsRunning = false;
            }

            if(event.type == SDL_EVENT_KEY_DOWN)
            {
                switch(event.key.key)
                {
                    case SDLK_ESCAPE:
                        m_MouseModeEnabled = true;
                        m_FreeCameraModeEnabled = false;
                        SDL_ShowCursor();
                        SDL_SetWindowRelativeMouseMode(m_Window->GetSDLWindow(),false);
                        break;
                    case SDLK_F4:
                        m_MouseModeEnabled = false;
                        m_FreeCameraModeEnabled = true;
                        SDL_HideCursor();
                        SDL_SetWindowRelativeMouseMode(m_Window->GetSDLWindow(),true);
                        break;
                    default:
                        break;       
                }
            }
            
            if(m_FreeCameraModeEnabled)
            {            
                if(event.type == SDL_EVENT_MOUSE_MOTION)
                {
                    float xoffset = static_cast<float>(event.motion.xrel);
                    float yoffset = static_cast<float>(event.motion.yrel); 

                    MouseRef->ProcessMotion(m_Camera, xoffset, -yoffset);
                }

                if(event.type == SDL_EVENT_MOUSE_WHEEL)
                {
                    float yoffset = static_cast<float>(event.wheel.y);

                    MouseRef->ProcessZoom(yoffset,m_Camera);
                }
            }
            
            if(m_MouseModeEnabled)
            {
                if(event.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
                {
                    if(!m_ImGuiLayer.isMouseOverGizmo() && event.button.button == SDL_BUTTON_LEFT)
                    {
                        MouseRef->ProcessMouseClick(mouseState.first,mouseState.second, m_Camera);
                    }
                }
                if(event.type == SDL_EVENT_KEY_DOWN)
                {
                    switch(event.key.key)
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

        m_lightingShader->Bind();
        m_lightingShader->setVec3("viewPos", m_Camera->GetPosition());
        
        SendAllLightsToShader();
        m_DirLight->SetUniforms(m_lightingShader);

        glm::mat4 view = m_Camera->GetViewMatrix(); // VIEW
        glm::mat4 projection = m_Camera->GetProjectionMatrix(); 


        for (auto& obj : m_SceneObjects)
        {
            obj->Render(m_Renderer, view, projection, m_lightingShader); 
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
        delete m_lightingShader;
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
            m_lightingShader->setVec3("point_lights[" + std::to_string(i) + "].position", m_LightObjects[i]->GetPosition());
            m_lightingShader->setVec3("point_lights[" + std::to_string(i) + "].ambient", m_LightObjects[i]->GetAmbientIntensity());
            m_lightingShader->setVec3("point_lights[" + std::to_string(i) + "].diffuse", m_LightObjects[i]->GetDiffuseIntensity());
            m_lightingShader->setVec3("point_lights[" + std::to_string(i) + "].specular", m_LightObjects[i]->GetSpecularIntensity());
            m_lightingShader->setFloat("point_lights["+ std::to_string(i) + "].constant", 1.0f);
            m_lightingShader->setFloat("point_lights["+ std::to_string(i) + "].linear", 0.09f);
            m_lightingShader->setFloat("point_lights["+ std::to_string(i) + "].quadratic", 0.032f);
        }
        m_lightingShader->setInt("numPointLights", numLights);
        // ----------------------------------------------------
    }

} // namespace isaacGraphicsEngine
