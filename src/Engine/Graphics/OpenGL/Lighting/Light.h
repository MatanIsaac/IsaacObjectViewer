#pragma once

#include <memory>
#include <string>
#include <glm/glm.hpp>
#include "Graphics/OpenGL/Primitives/Sphere.h"
#include "Graphics/OpenGL/Shaders/Shader.h"
#include "Graphics/OpenGL/Renderer/Renderer.h"
#include "Utility/config.h"  // for GetProjectRoot()
#include "Scene/ISceneObject.h"

namespace isaacObjectLoader
{
    class Light : public ISceneObject
    {
    public:
        Light(const glm::vec3& position, const glm::vec3& color);
        ~Light();

        void Update();
        // The 'sceneShader' parameter is the shader used to render scene objects,
        // which we update with light properties.
        virtual void Render(const Renderer& renderer, Shader& shader, const glm::mat4& view, const glm::mat4& projection) override;

        virtual std::size_t GetID() const { return m_ID; }
        virtual const std::string& GetName() const override { return m_Name; };
        virtual void SetName(const std::string& newName) override { m_Name = newName; }
        virtual ObjectType GetType() const { return ObjectType::Light; }
        
        float& GetSpecularIntensity() { return m_SpecularIntensity; }
        glm::vec3& GetPosition() override { return m_Sphere.GetPosition(); }
        glm::vec3& GetRotation() override { return m_Sphere.GetRotation(); }
        glm::vec3& GetScale() override { return m_Sphere.GetScale(); }
        glm::vec3& GetColor() { return m_Color; }

        void SetSpecularIntensity(float newSpecularIntensity) { m_SpecularIntensity = newSpecularIntensity; }
        void SetPosition(const glm::vec3& position) override { m_Sphere.SetPosition(position); }
        void SetRotation(const glm::vec3& rotation) override { m_Sphere.SetRotation(rotation); }
        void SetScale(const glm::vec3& scale) override { m_Sphere.SetScale(scale); }
        void SetColor(const glm::vec3& color) { m_Color = color; }
        
        bool IntersectRay(const Ray& ray, float* outDistance) const override { return m_Sphere.IntersectRay(ray, outDistance);}

        virtual std::size_t GenerateUniqueID() override
        {
            static std::size_t currentID = 0;
            return ++currentID;
        }

        virtual inline const VertexArray    &GetVertexArray()   const override { return m_Sphere.GetVertexArray(); }
        virtual inline const VertexBuffer   &GetVertexBuffer()  const override { return m_Sphere.GetVertexBuffer(); }
        virtual inline const IndexBuffer    &GetIndexBuffer()   const override { return m_Sphere.GetIndexBuffer(); }
        virtual unsigned inline int         GetIndexCount()     const override { return m_Sphere.GetIndexCount(); } 
    private:
        std::size_t m_ID;
        std::string m_Name; 
        
        glm::vec3 m_Color;
        Sphere m_Sphere;
        float m_SpecularIntensity;
        std::unique_ptr<Shader> m_Shader;
    };
}
