#pragma once

#include "Utility/config.h"
#include "Scene/ISceneObject.h"
#include <ImGuizmo.h>
#include "ImGuiFileDialog/ImGuiFileDialog.h"

namespace isaacObjectViewer
{
    class Engine; 

    enum GizmoMode : int
    {
        NONE = 0,
        TRANSLATION = ImGuizmo::TRANSLATE,
        ROTATION = ImGuizmo::ROTATE,
        SCALE = ImGuizmo::SCALE
    };    

    enum class PendingTexSlot 
    { 
        NONE, 
        DIFFUSE, 
        SPECULAR 
    };

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

        void SetGizmoOperation(GizmoMode mode) { m_GizmoOperation = mode; }
        void ResetSelectedObject() {  selected = nullptr; } 
        bool isMouseOverGizmo()
        {
            return ImGuizmo::IsOver((ImGuizmo::OPERATION)m_GizmoOperation);
        }
    private:
        void DrawTopPanel(Engine* engine);    
        void DrawRightPanel(Engine* engine);
        void DrawSceneHierarchyPanel(Engine* engine);    
        void DrawSettings(ISceneObject* selected);
        void LoadFont();
        void DrawGizmos(Engine* engine,int gizmoOperation);

    private:
        SDL_Window* m_Window = nullptr;
        void* m_GLContext = nullptr;
        ISceneObject* selected = nullptr;
        
        int m_GizmoOperation;
        
        ImGuiFileDialog m_ImportObjectDialog;
        ImGuiFileDialog m_DiffuseFileDialog;
        ImGuiFileDialog m_SpecularFileDialog;
        std::string m_CurrentPath  = std::filesystem::current_path().string();
        std::string m_SelectedPath;     
        const char* m_ImageDialogFilters = "All Images{.png,.jpg,.jpeg},.png,.jpg,.jpeg";

    };
}