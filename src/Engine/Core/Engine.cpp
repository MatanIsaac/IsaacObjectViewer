#include "Engine.h"
#include <algorithm>
#include "Utility/ColorMacros.h"
#include "Utility/Log.hpp"
#include "Mouse.h"
#include "../UI/ImGuiLayer.h"
#include <utility>

namespace isaacObjectLoader
{
    inline Mouse* MouseRef = Mouse::GetInstance();
    Engine *Engine::s_Instance = nullptr;

    Engine::Engine()
        : m_Window(nullptr),
          m_Shader(nullptr),
          m_lightingShader(nullptr),
          m_Cubes({}), 
          m_Light(nullptr),
          m_Camera(nullptr),
          m_SelectedObject(nullptr),
          m_MouseModeEnabled(true),
          m_FreeCameraModeEnabled(false),
          m_KeyPressed(false),
          m_IsRunning(false)
    {
        
    }

    void Engine::Run(bool fullscreen)
    {
        assert(Init("Isaac's 3D Object Loader", SCREEN_WIDTH, SCREEN_HEIGHT, fullscreen));
        
        ImGuiLayer imgui;
        imgui.Init(m_Window->GetSDLWindow(),m_Window->GetGLContext());

        const float targetFrameTime = 1.0f / 60.0f; // Target frame time for 60 FPS
        while (m_IsRunning)
        {
            // per-frame time logic
            // --------------------
            float frameStartTime = static_cast<float>(SDL_GetTicks()) / 1000.0f;
            
            // Rendering Correctly

            imgui.Begin();
            imgui.DrawUI();
            
            // input
            ProcessInput();
            Update(m_DeltaTime);
            Render();

            float frameEndTime  = static_cast<float>(SDL_GetTicks()) / 1000.0f;
            float frameDuration = frameEndTime - frameStartTime;

            if (frameDuration < targetFrameTime)
            {
                SDL_Delay(static_cast<Uint32>((targetFrameTime - frameDuration) * 1000.0f));
            }

            m_DeltaTime = static_cast<float>(SDL_GetTicks()) / 1000.0f - frameStartTime;
            if (m_DeltaTime > 0.0f)
                m_FPS = 1.0f / m_DeltaTime;
            
            imgui.End();
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

        m_Light     = new Light({1.2f, 1.0f, 2.0f},{1.0f, 1.0f, 1.0f});

        m_SceneObjects.push_back(m_Light->GetSceneObject());

        std::string colors_vs  = "src/Resources/Shaders/colors.vs"; 
        std::string colors_fs  = "src/Resources/Shaders/colors.fs"; 
        ConvertSeparators(colors_vs);
        ConvertSeparators(colors_fs);
        

        m_lightingShader = new Shader(colors_vs.c_str(), colors_fs.c_str());
        m_lightingShader->Bind();
                                        
        m_BackgroundColor = glm::vec4(0.75f, 0.75f, 0.75f, 1.0f);

        int display_w, display_h;
        SDL_GetWindowSizeInPixels(m_Window->GetSDLWindow(), &display_w, &display_h);

        m_Camera = new Camera(glm::vec3(0.0f, 2.0f, 5.0f));
        m_Camera->SetProjection(m_Camera->GetZoom(),(float)display_w / (float)display_h, 0.1f,100.0f);
        
        
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
                    case SDLK_BACKSPACE:
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
                    
                    if(event.button.button == SDL_BUTTON_LEFT)
                    {
                        MouseRef->ProcessMouseClick(mouseState.first,mouseState.second, m_Camera);
                    }
                }
            }
        }
    }

    // @brief updates all of the engine dependencies, resources and objects.
    void Engine::Update(float dt) 
    {

        //LOG_INFO("DeltaTime: {0}, FPS: {1}",dt,m_FPS);
        if(!m_MouseModeEnabled)
            m_Camera->Update(dt);
    }

    // @brief renders all of the engine textures, sounds and objects.
    void Engine::Render()
    {
        // Rendering
        int display_w, display_h;
        SDL_GetWindowSizeInPixels(m_Window->GetSDLWindow(), &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);

        // render
        glClearColor(m_BackgroundColor.x, m_BackgroundColor.y, m_BackgroundColor.z, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        m_lightingShader->Bind();
        m_lightingShader->setVec3("viewPos", m_Camera->GetPosition());
                
        glm::mat4 view = m_Camera->GetViewMatrix(); // VIEW
        glm::mat4 projection = m_Camera->GetProjectionMatrix(); 
                                                
        // Render Primitives
        //------------------------------------------------------------------------------------
        for (auto& cube : m_Cubes)
        {
            cube->Render(m_Renderer,*m_lightingShader, view, projection);
        }
        
        //------------------------------------------------------------------------------------

        // render light
        m_Light->Render(m_Renderer,*m_lightingShader, view, projection);
    }

    // @brief cleans all of the engine resources.
    void Engine::Clean()
    {
        ClearSceneObjects();
        ClearCubes();
        delete m_Shader;
        delete m_Window;
        SDL_Quit();
    }

    // @brief loads up needed resources such as textures, sfx, music etc..
    bool Engine::LoadResources() { return false; }

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
            m_SceneObjects.push_back(newCube);
        }
    }    

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
        


} // namespace isaacGraphicsEngine
