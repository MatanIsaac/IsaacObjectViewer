#pragma once

#include "Utility/config.h"

#include "Graphics/OpenGL/Renderer/Renderer.h"
#include "ISceneObject.h"

namespace isaacObjectLoader
{
    class Circle : public ISceneObject
    {
    public:
        Circle(const glm::vec3& position = DEFAULT_POSITION);
        ~Circle() override;
    
        void Render(const Renderer& renderer, Shader& shader, const glm::mat4& view, const glm::mat4& projection) override;
        
        std::size_t GetID() const { return m_ID; }
        const std::string& GetName() const override { return m_Name; };
        void SetName(const std::string& newName) override { m_Name = newName; }
        ObjectType GetType() const override { return ObjectType::Circle; }

        glm::vec3& GetPosition() override { return m_Position; }
        glm::vec3& GetRotation() override { return m_Rotation; }
        glm::vec3& GetScale() override { return m_Scale; }
        glm::vec3& GetColor() { return m_Color; }
        glm::mat4 GetModelMatrix() const;


        void ResetPosition() { m_Position = DEFAULT_POSITION; }
        void ResetRotation() { m_Rotation = DEFAULT_ROTATION; }
        void ResetScale() { m_Scale = DEFAULT_SCALE; }
        inline void SetPosition(const glm::vec3& newPosition) override { m_Position = newPosition; }
        inline void SetRotation(const glm::vec3& rotation) override { m_Rotation = rotation; }
        inline void SetScale(const glm::vec3& scale) override { m_Scale = scale; }
        inline void SetColor(const glm::vec3& newColor) { m_Color = newColor; }

        inline const VertexArray    &GetVertexArray()   const override { return *m_VertexArray; }
        inline const VertexBuffer   &GetVertexBuffer()  const override { return *m_VertexBuffer; }
        inline const IndexBuffer    &GetIndexBuffer()   const override { return *m_IndexBuffer; }
        inline unsigned int         GetIndexCount()     const override { return m_IndicesCount; }
        
        bool IntersectRay(const Ray& ray, float* outDistance) const;

        virtual inline std::size_t GenerateUniqueID() override
        {
            static std::size_t cubeID = 0;
            return ++cubeID;
        }

    private:
        std::size_t m_ID;
        std::string m_Name;
        
        glm::vec3 m_Position;
        glm::vec3 m_Rotation;
        glm::vec3 m_Scale;
        glm::vec3 m_Color;

        Shader* m_CircleShader;

        std::unique_ptr<IndexBuffer> m_IndexBuffer;
        std::unique_ptr<VertexBuffer> m_VertexBuffer;
        std::unique_ptr<VertexArray> m_VertexArray;
        int m_IndicesCount;

    };
}