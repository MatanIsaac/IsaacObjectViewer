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
        public:
        /// @brief Default values for the object's properties.
        static constexpr glm::vec3 DEFAULT_POSITION = {0.0f, 0.0f, 0.0f};
        static constexpr glm::vec3 DEFAULT_ROTATION = {0.0f, 0.0f, 0.0f};
        static constexpr glm::vec3 DEFAULT_SCALE = {1.0f, 1.0f, 1.0f};
        static constexpr glm::vec3 DEFAULT_COLOR = {1.0f, 0.5f, 0.31f};

        /// @brief Virtual destructor for the IRenderable interface.
        virtual ~IRenderable() = default;

        /// @brief Renders the object.
        /// @param renderer The renderer to use.
        /// @param view The view matrix.
        /// @param projection The projection matrix.
        /// @param shader The shader to use (optional).
        virtual void Render(const Renderer& renderer, const glm::mat4& view, const glm::mat4& projection, Shader* shader = nullptr) = 0;

        /// @brief Gets the ID of the object.
        /// @return The ID of the object.
        virtual std::size_t GetID() const = 0;
        
        /// @brief Gets the name of the object.
        /// @return The name of the object.
        virtual const std::string& GetName() const = 0;

        /// @brief Sets the name of the object.
        /// @param name The new name of the object.
        virtual void SetName(const std::string& name) = 0;

        /// @brief Gets the type of the object.
        /// @return The type of the object.
        virtual ObjectType GetType() const = 0;

        /// @brief Gets the position of the object.
        /// @return The position of the object.
        virtual glm::vec3& GetPosition() = 0;

        /// @brief Gets the rotation of the object.
        /// @return The rotation of the object.
        virtual glm::vec3& GetRotation() = 0;

        /// @brief Gets the orientation of the object.
        /// @return The orientation of the object.
        virtual glm::quat& GetOrientation() = 0;

        /// @brief Gets the scale of the object.
        /// @return The scale of the object.
        virtual glm::vec3& GetScale() = 0;

        /// @brief Gets the color of the object.
        /// @return The color of the object.
        virtual glm::vec3& GetColor() = 0;

        /// @brief Gets the material usage flag of the object.
        /// @return The material usage flag of the object.
        virtual bool& GetUseMaterial() = 0;

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

        /// @brief Sets the color of the object.
        /// @param newColor The new color of the object.
        virtual void SetColor(const glm::vec3& newColor) = 0;

        /// @brief Sets the material usage flag of the object.
        /// @param useMaterial The new material usage flag of the object.
        virtual void SetUseMaterial(bool useMaterial) = 0;

        /// @brief Gets the model matrix of the object.
        /// @return The model matrix of the object.
        glm::mat4 GetModelMatrix()
        {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), GetPosition());
            model *= glm::toMat4(GetOrientation());
            model = glm::scale(model, GetScale());
            return model;
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

        /// @brief Gets the default material for the object.
        /// @return The default material for the object.
        Material GetDefaultMaterial()
        {
            std::string diffuseMap = GetProjectRootPath("/src/Resources/Textures/wood-02/WoodOak_2K_albedo.png");
            std::string specularMap = GetProjectRootPath("/src/Resources/Textures/wood-02/WoodOak_2K_roughness.png");

            auto diffuse = TextureManager::LoadTexture(diffuseMap,TextureType::DIFFUSE);
            auto specular = TextureManager::LoadTexture(specularMap,TextureType::SPECULAR);

            return Material(std::shared_ptr<Texture>(diffuse),std::shared_ptr<Texture>(specular),32.f);
        }
    };
}