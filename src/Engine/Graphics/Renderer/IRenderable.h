/**
 * @file IRenderable.h
 * @brief Header file for the IRenderable interface.
 * This interface defines the methods required for a renderable object.
 */

#pragma once
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>

#include "Renderer.h"
#include "Graphics/Shader/Shader.h"
#include "Graphics/TextureManager.h"
#include "Graphics/Material.h"

namespace isaacObjectViewer
{
    /// @brief Enumeration of object types.
    enum class ObjectType : int
    {
        Unknown = 0, // default value for uninitialized/legacy objects
        Cube,
        Sphere,
        Cylinder,
        Plane,
        PointLight,
        Imported,
        Count 
    };

    class IRenderable
    {
    private: 
        
        // unlit condition
        bool isUnlit = false;
        std::unique_ptr<Shader> m_UnlitShader;

    public:
        /// @brief Default values for the object's properties.
        static constexpr glm::vec3 DEFAULT_POSITION = {0.0f, 0.0f, 0.0f};
        static constexpr glm::vec3 DEFAULT_ROTATION = {0.0f, 0.0f, 0.0f};
        static constexpr glm::vec3 DEFAULT_SCALE = {1.0f, 1.0f, 1.0f};
        static constexpr glm::vec3 DEFAULT_COLOR = {1.0f, 0.5f, 0.31f};

        /// @brief Virtual destructor for the IRenderable interface.
        virtual ~IRenderable() = default;

        // Rendering
        // ---------------------------------------------------------
        /// @brief Renders the object.
        /// @param renderer The renderer to use.
        /// @param view The view matrix.
        /// @param projection The projection matrix.
        /// @param shader The shader to use (optional).
        virtual void Render(const Renderer& renderer, const glm::mat4& view, const glm::mat4& projection, std::unique_ptr<Shader>& shader) = 0;

        // Identification
        // ---------------------------------------------------------
        /// @brief Gets the ID of the object.
        /// @return The ID of the object.
        virtual const std::size_t& GetID() const = 0;
        
        /// @brief Gets the name of the object.
        /// @return The name of the object.
        virtual const std::string& GetName() const = 0;
        
        /// @brief Sets the name of the object.
        /// @param name The new name of the object.
        virtual void SetName(const std::string& name) = 0;
        
        /// @brief Gets the type of the object.
        /// @return The type of the object.
        virtual const ObjectType& GetType() const = 0;

        /// @brief Generates a unique ID for the scene object.
        /// @return The unique ID.
        virtual std::size_t GenerateUniqueID() = 0;
        // ---------------------------------------------------------
        
        // Transform
        // ---------------------------------------------------------
        // Getters
        /// @brief Gets the position of the object.
        /// @return The position of the object.
        virtual const glm::vec3& GetPosition() const = 0;

        /// @brief Gets the rotation of the object.
        /// @return The rotation of the object.
        virtual const glm::vec3& GetRotation() const = 0;

        /// @brief Gets the orientation of the object.
        /// @return The orientation of the object.
        virtual const glm::quat& GetOrientation() const = 0;

        /// @brief Gets the scale of the object.
        /// @return The scale of the object.
        virtual const glm::vec3& GetScale() const = 0;

        // Setters
        /// @brief Sets the position of the object.
        /// @param newPosition The new position of the object.
        virtual void SetPosition(const glm::vec3& newPosition) = 0;

        /// @brief Sets the rotation of the object.
        /// @param newRotation The new rotation of the object.
        virtual void SetRotation(const glm::vec3& newRotation) = 0;

        /// @brief Sets the orientation of the object.
        /// @param newOrientation The new orientation of the object.
        virtual void SetOrientation(const glm::quat& newOrientation) = 0;

        /// @brief Sets the scale of the object.
        /// @param newScale The new scale of the object.
        virtual void SetScale(const glm::vec3& newScale) = 0;

        /// @brief Sets the rotation of the object using Euler angles.
        /// @param eulerDegrees The new rotation of the object in Euler angles (degrees).
        //virtual void SetRotationEuler(const glm::vec3& eulerDegrees) = 0;

        /// @brief Gets the rotation of the object in Euler angles.
        /// @return The rotation of the object in Euler angles (degrees).
        //virtual glm::vec3 GetRotationEuler() const = 0;
        // ---------------------------------------------------------
        
        // Appearance        
        // ---------------------------------------------------------
        /// @brief Gets the color of the object.
        /// @return The color of the object.
        virtual const glm::vec3& GetColor() const = 0;

        /// @brief Gets the material of the object.
        /// @return The material of the object.
        virtual const Material& GetMaterial() const = 0;

        /// @brief Gets the material usage flag of the object.
        /// @return The material usage flag of the object.
        virtual const bool& GetUseMaterial() const = 0;

        /// @brief Gets the shininess of the material.
        virtual const float& GetShininess() const = 0;

        /// @brief Sets the color of the object.
        /// @param newColor The new color of the object.
        virtual void SetColor(const glm::vec3& newColor) = 0;

        /// @brief Sets the material of the object.
        /// @param material The new material of the object.
        virtual void SetMaterial(const Material& material) = 0;

        /// @brief Sets the material usage flag of the object.
        /// @param useMaterial The new material usage flag of the object.
        virtual void SetUseMaterial(bool useMaterial) = 0;

        /// @brief Sets the shininess of the material.
        /// @param shininess The new shininess of the material.
        virtual void SetShininess(float shininess) = 0;

        /// @brief Sets the texture filter mode for all textures in the material.
        /// @param filterMode The new texture filter mode.
        virtual void SetFilterMode([[maybe_unused]] TextureFilterMode filterMode) {}
        
        /// @brief Sets whether the object is unlit.
        /// @param unlit True if the object is unlit, false otherwise.
        virtual void DisableUnlit() 
        { 
            isUnlit = false; 
        }

        /// @brief Enables the unlit shader for the object.
        /// @param vertexPath The path to the vertex shader.
        /// @param fragmentPath The path to the fragment shader.
        /// @return A unique pointer to the created unlit shader, or nullptr if creation failed.
        /// @note Ownership of the created shader is transferred to the caller.
        virtual bool EnableUnlit(const std::string& vertexPath, const std::string& fragmentPath) 
        { 
            isUnlit = true; 
            m_UnlitShader = std::make_unique<Shader>(vertexPath.c_str(), fragmentPath.c_str());
            if(m_UnlitShader == nullptr)
            {
                LOG_ERROR("Failed to create parse unlit shader!\n");
                return false;
            }
            return true;
        }

        /// @brief Gets whether the object is unlit.
        /// @return True if the object is unlit, false otherwise.
        virtual const bool& IsUnlit() const { return isUnlit; }

        /// @brief Gets the unlit shader of the object.
        /// @return A unique pointer to the unlit shader.
        virtual std::unique_ptr<Shader>& GetUnlitShader() { return m_UnlitShader; }

        // ---------------------------------------------------------

        // Helpers
        // ---------------------------------------------------------
        /// @brief Gets the model matrix of the object.
        /// @return The model matrix of the object.
        glm::mat4 GetModelMatrix()
        {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), GetPosition());
            model *= glm::toMat4(GetOrientation());
            model = glm::scale(model, GetScale());
            return model;
        }

        /// @brief Sets the normal matrix uniform for a given shader.
        /// @param shader The shader to set the uniform for.
        /// @param view The current view matrix.
        void SetNormalMatrixUniform(std::unique_ptr<Shader>& shader, const glm::mat4& view)
        {
            glm::mat4 model = GetModelMatrix();
            glm::mat4 normalMatrix = glm::transpose(glm::inverse(view * model));
            shader->setMat4("normalMatrix", normalMatrix);
        }

        /// @brief Sets the transform of the object from a matrix.
        /// @param mat The transformation matrix.
        void SetTransformFromMatrix(const glm::mat4& mat) 
        {
            glm::vec3 scale, translation, skew;
            glm::vec4 perspective;
            glm::quat orientation;
            glm::decompose(mat, scale, orientation, translation, skew, perspective);
            this->SetPosition(translation);
            this->SetScale(scale);
            this->SetOrientation(orientation);
            this->SetRotation(glm::degrees(glm::eulerAngles(orientation)));
        }
        // ---------------------------------------------------------
    };
}