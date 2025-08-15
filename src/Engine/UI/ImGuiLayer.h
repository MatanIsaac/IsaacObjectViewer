/**
 * @brief ImGui Layer for the application.
 * This layer handles the ImGui interface and user interactions.
 * It is responsible for rendering the UI elements and handling user input.
 */

#pragma once

#include "Utility/config.h"
#include "Core/IObject.h"
#include <ImGuizmo.h>
#include "ImGuiFileDialog/ImGuiFileDialog.h"

namespace isaacObjectViewer
{
    class Engine; 

    /// @brief Enumeration for gizmo modes.
    enum GizmoMode : int
    {
        NONE = 0,
        TRANSLATION = ImGuizmo::TRANSLATE,
        ROTATION = ImGuizmo::ROTATE,
        SCALE = ImGuizmo::SCALE
    };    

    /// @brief Enumeration for pending texture slots.
    enum class PendingTexSlot 
    { 
        NONE, 
        DIFFUSE, 
        SPECULAR 
    };

    class ImGuiLayer
    {
    public:
        /// @brief Constructor.
        ImGuiLayer();

        /// @brief Destructor.
        ~ImGuiLayer();

        /// @brief Initializes the ImGui layer.
        void Init(SDL_Window* window, void* gl_context);
        
        /// @brief Begins the ImGui frame.
        void Begin();

        /// @brief Ends the ImGui frame.
        void End();

        /// @brief Shuts down the ImGui layer.
        void Shutdown();

        /// @brief Draws the ImGui UI.
        void DrawUI();

        /// @brief Sets the gizmo operation mode.
        void SetGizmoOperation(GizmoMode mode) { m_GizmoOperation = mode; }

        /// @brief Resets the selected object.
        void ResetSelectedObject() {  selected = nullptr; } 

        /// @brief Checks if the mouse is over the gizmo.
        bool isMouseOverGizmo()
        {
            return ImGuizmo::IsOver((ImGuizmo::OPERATION)m_GizmoOperation);
        }

        /// @brief Checks if the mouse is over the UI.
        bool isMouseOverUI() const { return m_IsMouseOverUI; }

    private:
        /// @brief Draws the top panel.
        /// @param engine The engine instance.
        void DrawTopPanel(Engine* engine);    

        /// @brief Draws the right panel.
        /// @param engine The engine instance.
        void DrawRightPanel(Engine* engine);

        /// @brief Draws the scene hierarchy panel.
        /// @param engine The engine instance.
        void DrawSceneHierarchyPanel(Engine* engine);

        /// @brief Draws the settings panel.
        /// @param selected The selected scene object.
        void DrawSettings(IObject* selected);

        /// @brief Loads the font.
        void LoadFont();

        /// @brief Draws the gizmos.
        /// @param engine The engine instance.
        /// @param gizmoOperation The gizmo operation mode.
        void DrawGizmos(Engine* engine,int gizmoOperation);

    private:
        SDL_Window* m_Window = nullptr;
        void* m_GLContext = nullptr;
        IObject* selected = nullptr;
        
        int m_GizmoOperation;
        
        ImGuiFileDialog m_ImportObjectDialog;
        ImGuiFileDialog m_DiffuseFileDialog;
        ImGuiFileDialog m_SpecularFileDialog;
        std::string m_CurrentPath;
        std::string m_SelectedPath;     
        const char* m_ImageDialogFilters;
        const char* m_ImportObjDialogFilters;

        bool m_IsMouseOverUI;

        float M_RightPanelWidth; 
        float M_TopPanelHeight;

    };
}