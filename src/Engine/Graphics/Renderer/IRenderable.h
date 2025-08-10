#pragma once
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>

#include "Renderer.h"
#include "Graphics/Shaders/Shader.h"



namespace isaacObjectViewer
{
    class IRenderable
    {
        public:
        virtual ~IRenderable() = default;
        
        virtual void Render(const Renderer& renderer, const glm::mat4& view, const glm::mat4& projection, Shader* shader = nullptr) = 0;
    };
}