/**
 *  @file Cube.h
 *  @brief Header file for the Cube class.
 *   This class represents a cube primitive in the scene.
 */

#pragma once

#include "Utility/config.h"
#include <memory>

#include "Graphics/Buffers/IndexBuffer.h"
#include "Graphics/Buffers/VertexArray.h"
#include "Graphics/Buffers/VertexBuffer.h"
#include "Graphics/Buffers/VertexBufferLayout.h"
#include "Graphics/Shader/Shader.h"
#include "Graphics/Renderer/Renderer.h"
#include "IObject.h"
#include "Texture.h"
#include "Material.h"

namespace isaacObjectViewer
{
    class Cube : public IObject
    {
    public:

        /// @brief Constructs a Cube.
        /// @param position The position of the cube.
        Cube(const glm::vec3& position = DEFAULT_POSITION);

        /// @brief Destroys the Cube.
        ~Cube() override;

        /// @brief Updates the Cube.
        void Update();

        /// @brief Renders the Cube.
        /// @param renderer The renderer to use for rendering.
        /// @param view The view matrix.
        /// @param projection The projection matrix.
        /// @param shader The shader to use for rendering.
        void Render(const Renderer& renderer, const glm::mat4& view, const glm::mat4& projection, std::unique_ptr<Shader>& shader) override;

        /// @brief Gets the cube's ID.
        /// @return The cube's ID.
        const std::size_t& GetID() const { return m_ID; }

        /// @brief Gets the cube's name.
        /// @return The cube's name.
        const std::string& GetName() const override { return m_Name; };

        
        /// @brief Gets the cube's type.
        /// @return The cube's type.
        const ObjectType& GetType() const override { return m_Type; }
        
        /// @brief Gets the cube's position.
        /// @return The cube's position.
        const glm::vec3& GetPosition() const override { return m_Position; }

        /// @brief Gets the cube's rotation.
        /// @return The cube's rotation.
        const glm::vec3& GetRotation() const override { return m_Rotation; }

        /// @brief Gets the cube's orientation.
        /// @return The cube's orientation.
        const glm::quat& GetOrientation() const override { return m_Orientation; }

        /// @brief Gets the cube's scale.
        /// @return The cube's scale.
        const glm::vec3& GetScale() const override { return m_Scale; }

        /// @brief Gets the cube's color.
        /// @return The cube's color.
        const glm::vec3& GetColor() const override { return m_Color; }

        /// @brief Gets the cube's material.
        /// @return The cube's material.
        const Material& GetMaterial() const override { return m_Material; }

        /// @brief Gets the cube's material usage flag.
        /// @return The cube's material usage flag.
        const bool& GetUseMaterial() const override { return m_UseMaterial; }

        /// @brief Gets the cube's shininess.
        /// @return The cube's shininess.
        const float& GetShininess() const override { return m_Material.Shininess; }

        /// @brief Resets the cube's position to the default.
        void ResetPosition() { m_Position = DEFAULT_POSITION; }
        
        /// @brief Resets the cube's rotation to the default.
        void ResetRotation() { m_Rotation = DEFAULT_ROTATION; }
        
        /// @brief Resets the cube's scale to the default.
        void ResetScale() { m_Scale = DEFAULT_SCALE; }
        
        /// @brief Sets the cube's name.
        /// @param newName The new name for the cube.
        void SetName(const std::string& newName) override { m_Name = newName; }

        /// @brief Sets the cube's position.
        /// @param newPosition The new position for the cube.
        inline void SetPosition(const glm::vec3& newPosition) override { m_Position = newPosition; }

        /// @brief Sets the cube's rotation.
        /// @param rotation The new rotation for the cube.
        inline void SetRotation(const glm::vec3& rotation) override { m_Rotation = rotation; }

        /// @brief Sets the cube's orientation.
        /// @param newOrientation The new orientation for the cube.
        inline void SetOrientation(const glm::quat& newOrientation) override { m_Orientation = newOrientation; }

        /// @brief Sets the cube's scale.
        /// @param scale The new scale for the cube.
        inline void SetScale(const glm::vec3& scale) override { m_Scale = scale; }

        /// @brief Sets the cube's color.
        /// @param newColor The new color for the cube.
        inline void SetColor(const glm::vec3& newColor) override{ m_Color = newColor; }

        /// @brief Sets the cube's material.
        /// @param newMaterial The new material for the cube.
        void SetMaterial(const Material& newMaterial) override { m_Material = newMaterial; }
        
        /// @brief Sets the cube's material usage flag.
        /// @param use The new material usage flag for the cube.
        inline void SetUseMaterial(bool use) override { m_UseMaterial = use; }

        /// @brief Sets the cube's shininess.
        /// @param newShininess The new shininess for the cube's material.
        void SetShininess(float newShininess) override { m_Material.Shininess = newShininess; }

        /// @brief Checks for intersection with a ray.
        /// @param ray The ray to check for intersection.
        /// @param outDist The distance to the intersection point, if any.
        /// @return True if the ray intersects the cube, false otherwise.
        bool IntersectRay(const Ray& ray, float* outDist) override
        {
            glm::vec3 halfScale = m_Scale * 0.5f;
            glm::vec3 boxMin = m_Position - halfScale;
            glm::vec3 boxMax = m_Position + halfScale;
            return RayIntersectsAABB(ray, boxMin, boxMax, outDist);
        }

        /// @brief Generates a unique ID for the cube.
        /// @return The unique ID for the cube.
        inline std::size_t GenerateUniqueID() override
        {
            static std::size_t cubeID = 0;
            return ++cubeID;
        }
        
    private:
        std::size_t m_ID;
        std::string m_Name;
        ObjectType m_Type;
        glm::vec3 m_Position;
        glm::vec3 m_Rotation;
        glm::quat m_Orientation;
        glm::vec3 m_Scale;
        glm::vec3 m_Color;
        bool m_UseMaterial;

        Material m_Material;

        std::unique_ptr<IndexBuffer> m_IndexBuffer;
        std::unique_ptr<VertexBuffer> m_VertexBuffer;
        std::unique_ptr<VertexArray> m_VertexArray;

        int m_IndicesCount; // will be set to 36 (6 faces * 6 indices)

        static constexpr int m_FloatsPerVertex = 8;
        unsigned int m_VertexCount = 24; 

    public:
        /// @brief The cube's vertex data.
        /// 24 vertices (4 per face) × 6 floats each = 144 floats.
        /// Each vertex: pos(3) | normal(3) | uv(2) | tangent(3) | bitangent(3)
        /// @return A pointer to the cube's vertex data.
        static constexpr float m_CubeVertices[] = 
        {
            // Front face (z = -0.5, normal (0,0,-1))
            -0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,  0.0f, 0.0f,  1,0,0,   0,1,0, // bottom-left
             0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,  1.0f, 0.0f,  1,0,0,   0,1,0, // bottom-right
             0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,  1.0f, 1.0f,  1,0,0,   0,1,0, // top-right
            -0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,  0.0f, 1.0f,  1,0,0,   0,1,0, // top-left

            // Back face (z = 0.5, normal (0,0,1))
            -0.5f, -0.5f,  0.5f,   0.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1,0,0, 0,1,0, // bottom-left
             0.5f, -0.5f,  0.5f,   0.0f,  0.0f,  1.0f,  1.0f, 0.0f, 1,0,0, 0,1,0, // bottom-right
             0.5f,  0.5f,  0.5f,   0.0f,  0.0f,  1.0f,  1.0f, 1.0f, 1,0,0, 0,1,0, // top-right
            -0.5f,  0.5f,  0.5f,   0.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1,0,0, 0,1,0, // top-left

            // Left face (x = -0.5, normal (-1,0,0))
            -0.5f, -0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,  0.0f, 0.0f, 1,0,0, 0,1,0, // bottom-lef
            -0.5f, -0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1,0,0, 0,1,0, // bottom-rig
            -0.5f,  0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1,0,0, 0,1,0, // top-right
            -0.5f,  0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,  0.0f, 1.0f, 1,0,0, 0,1,0, // top-left

            // Right face (x = 0.5, normal (1,0,0))
             0.5f, -0.5f, -0.5f,   1.0f,  0.0f,  0.0f,  0.0f, 0.0f, 1,0,0, 0,1,0, // bottom-left
             0.5f, -0.5f,  0.5f,   1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1,0,0, 0,1,0, // bottom-right
             0.5f,  0.5f,  0.5f,   1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1,0,0, 0,1,0, // top-right
             0.5f,  0.5f, -0.5f,   1.0f,  0.0f,  0.0f,  0.0f, 1.0f, 1,0,0, 0,1,0, // top-left

            // Bottom face (y = -0.5, normal (0,-1,0))
            -0.5f, -0.5f,  0.5f,   0.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1,0,0, 0,1,0, // bottom-left
             0.5f, -0.5f,  0.5f,   0.0f, -1.0f,  0.0f,  1.0f, 0.0f, 1,0,0, 0,1,0, // bottom-right
             0.5f, -0.5f, -0.5f,   0.0f, -1.0f,  0.0f,  1.0f, 1.0f, 1,0,0, 0,1,0, // top-right
            -0.5f, -0.5f, -0.5f,   0.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1,0,0, 0,1,0, // top-left

            // Top face (y = 0.5, normal (0,1,0))
            -0.5f,  0.5f, -0.5f,   0.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1,0,0, 0,1,0, // bottom-left
             0.5f,  0.5f, -0.5f,   0.0f,  1.0f,  0.0f,  1.0f, 0.0f, 1,0,0, 0,1,0, // bottom-right
             0.5f,  0.5f,  0.5f,   0.0f,  1.0f,  0.0f,  1.0f, 1.0f, 1,0,0, 0,1,0, // top-right
            -0.5f,  0.5f,  0.5f,   0.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1,0,0, 0,1,0  // top-left
        };

        /// @brief The cube's index data.
        /// For each face (4 vertices) we define two triangles: (0,1,2) and (2,3,0)
        /// Since vertices for each face are sequential, we offset per face.
        /// @return A pointer to the cube's index data.
        static constexpr unsigned int m_CubeIndices[36] =
        {
            // Front face
             0,  1,  2,
             2,  3,  0,
            // Back face
             4,  5,  6,
             6,  7,  4,
            // Left face
             8,  9, 10,
            10, 11,  8,
            // Right face
            12, 13, 14,
            14, 15, 12,
            // Bottom face
            16, 17, 18,
            18, 19, 16,
            // Top face
            20, 21, 22,
            22, 23, 20
        };
    };
} // namespace isaacObjectViewer
