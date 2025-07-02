#include "Circle.h"

namespace isaacObjectLoader
{
    Circle::Circle(const glm::vec3 &position)
        : m_ID(GenerateUniqueID())
        , m_Name("Circle_" + std::to_string(m_ID))
        , m_Position(position)
        , m_Rotation(0.0f)
        , m_Scale(DEFAULT_SCALE)
        , m_Color(DEFAULT_COLOR)
    {
        // Set up VAO/VBO for a quad
        float quadVertices[] = {
            // x, y
            -0.5f, -0.5f,
             0.5f, -0.5f,
             0.5f,  0.5f,
            -0.5f,  0.5f
        };
        unsigned int quadIndices[] = {
            0, 1, 2,
            2, 3, 0
        };
        m_IndicesCount = 6;

        m_VertexArray = std::make_unique<VertexArray>();
        m_VertexBuffer = std::make_unique<VertexBuffer>(quadVertices, sizeof(quadVertices));
        VertexBufferLayout layout;
        layout.Push<float>(2); // 2D position
        m_VertexArray->AddBuffer(*m_VertexBuffer, layout);
        m_IndexBuffer = std::make_unique<IndexBuffer>(quadIndices, 6);

        std::string circle_vs  = "src/Resources/Shaders/circle.vs"; 
        std::string circle_fs  = "src/Resources/Shaders/circle.fs"; 
        ConvertSeparators(circle_vs);
        ConvertSeparators(circle_fs);

        // Compile shader from source (or load from file)
        m_CircleShader = new Shader(circle_vs.c_str(), circle_fs.c_str()); // Change as appropriate for your engine
    }

    Circle::~Circle() {}

    void Circle::Render(const Renderer& renderer, Shader& shader, const glm::mat4& view, const glm::mat4& projection)
    {
        m_CircleShader->Bind();

        glm::mat4 model = GetModelMatrix();
        
        glm::mat4 mvp = projection * view * model;
        m_CircleShader->setMat4("u_mvp", mvp);
        m_CircleShader->setVec3("objectColor", m_Color);

        renderer.Render(*m_VertexArray, *m_IndexBuffer, *m_CircleShader);
    }

    glm::mat4 Circle::GetModelMatrix() const
    {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, m_Position);
        model = glm::rotate(model, glm::radians(m_Rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(m_Rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, glm::radians(m_Rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, m_Scale);
        return model;
    }

    bool Circle::IntersectRay(const Ray& ray, float* outDistance) const
    {
        // 1. Circle plane: Assume Z is constant (circle lies in XY plane at m_Position.z)
        glm::vec3 circleCenter = m_Position;
        glm::vec3 circleNormal = glm::vec3(0.0f, 0.0f, 1.0f); // face +Z (edit if rotated!)

        // Ray: r(t) = origin + t*direction
        float denom = glm::dot(circleNormal, ray.GetDirection());
        if (fabs(denom) < 1e-6f)
            return false; // Parallel, no intersection

        float t = glm::dot(circleCenter - ray.GetOrigin(), circleNormal) / denom;
        if (t < 0) return false; // Intersection behind ray

        // 2. Intersection point
        glm::vec3 hitPoint = ray.GetOrigin() + t * ray.GetDirection();

        // 3. Is hitPoint inside the circle radius?
        float radius = m_Scale.x * 0.5f;
        float dist2 = glm::length(glm::vec2(hitPoint) - glm::vec2(circleCenter));
        if (dist2 > radius * radius)
            return false;

        if (outDistance) *outDistance = t;
        return true;
    }

}
