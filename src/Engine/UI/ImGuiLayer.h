#pragma once

#include "Utility/config.h"



namespace isaacObjectLoader
{
    class Engine; 

    class ImGuiLayer
    {
    public:
        ImGuiLayer();
        ~ImGuiLayer();
        
        void Init(SDL_Window* window, void* gl_context);
        void Begin();
        void End();
        void Shutdown();

        void DrawUI();
    private:
        void DrawTopPanel(Engine* engine);    
        void DrawRightPanel(Engine* engine);
        void DrawSceneHierarchyPanel(Engine* engine);    
        void LoadFont();
    private:
        SDL_Window* m_Window = nullptr;
        void* m_GLContext = nullptr;

    };
}