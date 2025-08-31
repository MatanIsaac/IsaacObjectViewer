// src/Engine/Graphics/Renderer/FrameStats.h
#pragma once
#include <cstdint>
#include "Utility/config.h"

namespace isaacObjectViewer
{
    struct FrameStats
    {
        uint32_t DrawCalls   = 0;
        uint64_t Triangles   = 0;
        uint64_t Vertices    = 0;

        void reset() noexcept { DrawCalls = 0; Triangles = 0; Vertices = 0; }

        // Count per draw submission
        void accumulate(GLenum mode, GLsizei count, GLsizei instanceCount = 1) noexcept 
        {
            if (count <= 0 || instanceCount <= 0) return;
            ++DrawCalls;

            // vertices = count * instances for both arrays and elements (count is index count in elements)
            Vertices += static_cast<uint64_t>(count) * static_cast<uint64_t>(instanceCount);

            uint64_t prim = 0;
            switch (mode) 
            {
            case GL_TRIANGLES:       prim = (count >= 3) ? (count / 3) : 0; break;
            case GL_TRIANGLE_STRIP:  prim = (count >= 3) ? (count - 2) : 0; break;
            case GL_TRIANGLE_FAN:    prim = (count >= 3) ? (count - 2) : 0; break;
            // non-triangle topologies contribute 0 triangles
            case GL_LINES:
            case GL_LINE_STRIP:
            case GL_POINTS:
            default: prim = 0; break;
            // If you use GL_PATCHES + tessellation, leave prim=0 or gate a GPU-query mode.
            }
            Triangles += prim * static_cast<uint64_t>(instanceCount);
        }
    };
}