#pragma once

#include <memory>
#include <string>
#include <glm/glm.hpp>
#include "Graphics/Primitives/Sphere.h"
#include "Graphics/Shaders/Shader.h"
#include "Graphics/Renderer/Renderer.h"
#include "Utility/config.h"  
#include "Scene/ISceneObject.h"

namespace isaacObjectViewer
{
    class PointLight : public ISceneObject
    {
    public:
        PointLight(const glm::vec3& position, const glm::vec3& color);
        ~PointLight();

        void Update();
        // The 'sceneShader' parameter is the shader used to render scene objects,
        // which we update with light properties.
        virtual void Render(const Renderer& renderer, const glm::mat4& view, const glm::mat4& projection, Shader* shader = nullptr) override;

        virtual std::size_t GetID() const { return m_ID; }
        virtual const std::string& GetName() const override { return m_Name; };
        virtual void SetName(const std::string& newName) override { m_Name = newName; }
        virtual ObjectType GetType() const { return ObjectType::PointLight; }
        
        glm::vec3& GetAmbientIntensity() { return m_AmbientIntensity; }
        glm::vec3& GetDiffuseIntensity() { return m_DiffuseIntensity; }
        glm::vec3& GetSpecularIntensity() { return m_SpecularIntensity; }
        glm::vec3& GetPosition() override { return m_Sphere.GetPosition(); }
        glm::vec3& GetRotation() override 
        { 
            static glm::vec3 zero = glm::vec3(0.0f); 
            return zero; 
        }

        glm::quat& GetOrientation() override 
        { 
            static glm::quat identity = glm::quat(1.0f, 0, 0, 0); 
            return identity;
        }
        glm::vec3& GetScale() override { return m_Sphere.GetScale(); }
        glm::vec3& GetColor() override { return m_Color; }
        bool& GetUseMaterial() override { return m_UseMaterial; }
        float& GetShininess() override { return m_Shininess; }


        void SetAmbientIntensity(const glm::vec3& newAmbientIntensity) { m_AmbientIntensity = newAmbientIntensity; }
        void SetDiffuseIntensity(const glm::vec3& newDiffuseIntensity) { m_DiffuseIntensity = newDiffuseIntensity; }
        void SetSpecularIntensity(const glm::vec3& newSpecularIntensity) { m_SpecularIntensity = newSpecularIntensity; }
        void SetPosition(const glm::vec3& position) override { m_Sphere.SetPosition(position); }
        void SetRotation(const glm::vec3& rotation) override {}
        void SetOrientation(const glm::quat& newOrientation) {}
        void SetDiffuseTexture(const std::shared_ptr<Texture>& tex) override { }
        void SetSpecularTexture(const std::shared_ptr<Texture>& tex) override { }
        
        void SetScale(const glm::vec3& scale) override { m_Sphere.SetScale(scale); }
        void SetColor(const glm::vec3& color) override { m_Color = color; }
        void SetUseMaterial(bool use) override { m_UseMaterial = use; }

        bool IntersectRay(const Ray& ray, float* outDistance) override { return m_Sphere.IntersectRay(ray, outDistance);}

        virtual std::size_t GenerateUniqueID() override
        {
            static std::size_t currentID = 0;
            return ++currentID;
        }

        virtual inline const VertexArray    &GetVertexArray()   const override { return m_Sphere.GetVertexArray(); }
        virtual inline const VertexBuffer   &GetVertexBuffer()  const override { return m_Sphere.GetVertexBuffer(); }
        virtual inline const IndexBuffer    &GetIndexBuffer()   const override { return m_Sphere.GetIndexBuffer(); }
        virtual unsigned inline int         GetIndexCount()     const override { return m_Sphere.GetIndexCount(); } 
        virtual unsigned inline int         GetVertexCount()    const override { return m_Sphere.GetVertexCount(); }
    private:
        std::size_t m_ID;
        std::string m_Name; 
        
        glm::vec3 m_Color;
        bool m_UseMaterial;
        Sphere m_Sphere;
        float m_Shininess = 0.f;

        glm::vec3 m_AmbientIntensity;
        glm::vec3 m_DiffuseIntensity;
        glm::vec3 m_SpecularIntensity;
        
        std::unique_ptr<Shader> m_Shader;
    };
}
