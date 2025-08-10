#pragma once

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>

#include <string>
#include "Core/Ray.h"
#include "Graphics/Buffers/IndexBuffer.h"
#include "Graphics/Buffers/VertexArray.h"
#include "Graphics/Buffers/VertexBuffer.h"
#include "Graphics/Renderer/Renderer.h"
#include "Graphics/Shaders/Shader.h"
#include "Graphics/Renderer/IRenderable.h"
#include "Graphics/Material.h"
#include "Graphics/Texture.h"
#include "Graphics/TextureManager.h"


namespace isaacObjectViewer
{
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

    class ISceneObject : public IRenderable
    {
    public:
        virtual ~ISceneObject() = default;

        virtual std::size_t GetID() const = 0;
        virtual const std::string& GetName() const = 0;
        virtual void SetName(const std::string& name) = 0;
        virtual ObjectType GetType() const = 0;
        
        virtual glm::vec3& GetPosition() = 0;
        virtual glm::vec3& GetRotation() = 0;
        virtual glm::quat& GetOrientation() = 0;
        virtual glm::vec3& GetScale() = 0;
        virtual float& GetShininess() = 0;
        virtual void SetPosition(const glm::vec3& newPosition) = 0;
        virtual void SetRotation(const glm::vec3& newRotation) = 0;
        virtual void SetOrientation(const glm::quat& newOrientation) = 0;
        virtual void SetScale(const glm::vec3& newScale) = 0;
        virtual void SetDiffuseTexture(const std::shared_ptr<Texture>& tex) = 0;
        virtual void SetSpecularTexture(const std::shared_ptr<Texture>& tex) = 0;

        static constexpr glm::vec3 DEFAULT_POSITION = {0.0f, 0.0f, 0.0f};
        static constexpr glm::vec3 DEFAULT_ROTATION = {0.0f, 0.0f, 0.0f};
        static constexpr glm::vec3 DEFAULT_SCALE = {1.0f, 1.0f, 1.0f};
        static constexpr glm::vec3 DEFAULT_COLOR = {1.0f, 0.5f, 0.31f};

        // returns true if hit and sets outdistance to the hit distance (if not null)
        virtual bool IntersectRay(const Ray& ray, float* outDistance) = 0;

        virtual std::size_t GenerateUniqueID() = 0;

        virtual inline const VertexArray    &GetVertexArray() const = 0;
        virtual inline const VertexBuffer   &GetVertexBuffer() const = 0;
        virtual inline const IndexBuffer    &GetIndexBuffer() const = 0;
        virtual unsigned inline int         GetIndexCount() const = 0; 

        glm::mat4 GetModelMatrix()
        {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), GetPosition());
            model *= glm::toMat4(GetOrientation());
            model = glm::scale(model, GetScale());
            return model;
        }

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