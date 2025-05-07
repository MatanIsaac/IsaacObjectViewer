#include "Engine.h"
#include <algorithm>
#include "Utility/ColorMacros.h"
#include "Utility/Log.hpp"
#include "Mouse.h"

namespace isaacGraphicsEngine
{
    inline Mouse& MouseRef = Mouse::GetInstance();
    Engine *Engine::s_Instance = nullptr;

    Engine::Engine()
        : m_Window(nullptr),
          m_Shader(nullptr),
          m_lightingShader(nullptr),
          m_Sphere(nullptr),
          m_Plane(nullptr),
          m_Cubes({}), 
          m_Cylinder(nullptr),
          m_Light(nullptr),
          m_Camera(nullptr),
          m_DisableInput(false),
          m_KeyPressed(false),
          m_IsRunning(false)
    {
        
    }

    void Engine::Run(bool fullscreen)
    {
        assert(Init("Isaac's Graphics Engine", SCREEN_WIDTH, SCREEN_HEIGHT, fullscreen));

        const float targetFrameTime = 1.0f / 60.0f; // Target frame time for 60 FPS
        while (m_IsRunning)
        {
            // per-frame time logic
            // --------------------
            float frameStartTime = static_cast<float>(SDL_GetTicks()) / 1000.0f;
            
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

        SDL_HideCursor();
        SDL_SetWindowRelativeMouseMode(m_Window->GetSDLWindow(),true);

        // Initialize glad
        //----------------------------------------------------------------------------------------
        if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
        {
            LOG_ERROR("Failed to load OpenGL!");
            SDL_Quit();
        }
        //----------------------------------------------------------------------------------------

        glEnable(GL_DEPTH_TEST); // Enable depth testing
	
        const glm::vec3 plane_pos = {5.f,1.f,1.f};
        const glm::vec3 sphere_pos = {-2.5f,1.f,1.f};
        m_Sphere    = new Sphere(sphere_pos);
        m_Plane     = new Plane(plane_pos);
        m_Cylinder  = new Cylinder();
        m_Light     = new Light({1.2f, 1.0f, 2.0f},{1.0f, 1.0f, 1.0f});

        std::string colors_vs  = "src/Resources/Shaders/colors.vs"; 
        std::string colors_fs  = "src/Resources/Shaders/colors.fs"; 
        ConvertSeparators(colors_vs);
        ConvertSeparators(colors_fs);

        m_lightingShader = new Shader(colors_vs.c_str(), colors_fs.c_str());
        m_lightingShader->Bind();
                                        
        m_BackgroundColor = glm::vec4(0.75f, 0.75f, 0.75f, 1.0f);

        m_Camera = new Camera(glm::vec3(0.0f, 2.0f, 5.0f));

        m_DisableInput = false;
        
        m_IsRunning = true;

        return true;
    }

    // @brief processes player input
    void Engine::ProcessInput()
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if(event.type == SDL_EVENT_QUIT)
            {
                m_IsRunning = false;
            }

            if(event.type == SDL_EVENT_KEY_DOWN)
            {
                switch(event.key.key)
                {
                    case SDLK_ESCAPE:
                        m_DisableInput = true;
                        SDL_ShowCursor();
                        SDL_SetWindowRelativeMouseMode(m_Window->GetSDLWindow(),false);
                        break;
                    case SDLK_BACKSPACE:
                        m_DisableInput = false;
                        SDL_HideCursor();
                        SDL_SetWindowRelativeMouseMode(m_Window->GetSDLWindow(),true);
                        break;
                    default:
                        break;       
                }
            }
            
            if(!m_DisableInput)
            {            
                if(event.type == SDL_EVENT_MOUSE_MOTION)
                {
                    float xoffset = static_cast<float>(event.motion.xrel);
                    float yoffset = static_cast<float>(event.motion.yrel); 

                    MouseRef.ProcessMotion(m_Camera, xoffset, -yoffset);
                }

                if(event.type == SDL_EVENT_MOUSE_WHEEL)
                {
                    float yoffset = static_cast<float>(event.wheel.y);

                    MouseRef.ProcessZoom(yoffset,m_Camera);
                }
            }
        }
    }

    // @brief updates all of the engine dependencies, resources and objects.
    void Engine::Update(float dt) 
    {

        LOG_INFO("DeltaTime: {0}, FPS: {1}",dt,m_FPS);
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
        glm::mat4 projection = glm::perspective(glm::radians(m_Camera->GetZoom()), // PROJECTION
                                                (float)display_w / (float)display_h,
                                                0.1f,
                                                100.0f);
                                                
        // Render Primitives
        //------------------------------------------------------------------------------------
        m_Sphere->Render(m_Renderer, *m_lightingShader, view, projection);
        m_Plane->Render(m_Renderer, *m_lightingShader, view, projection);
        m_Cylinder->Render(m_Renderer, *m_lightingShader, view, projection);
        
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

        ClearCubes();
        delete m_Sphere;
        delete m_Plane;
        delete m_Cylinder;
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
        }
    }    



} // namespace isaacGraphicsEngine
