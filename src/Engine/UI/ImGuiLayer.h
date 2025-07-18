#pragma once

#include "Utility/config.h"
#include "Scene/ISceneObject.h"
#include <ImGuizmo.h>
namespace isaacObjectViewer
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
        void DrawSettings(Engine* engine, ISceneObject* selected);
        void LoadFont();
        void DrawGizmos(Engine* engine,int gizmoOperation);
    private:
        SDL_Window* m_Window = nullptr;
        void* m_GLContext = nullptr;
        ISceneObject* selected = nullptr;

        int m_GizmoOperation;
    };
}