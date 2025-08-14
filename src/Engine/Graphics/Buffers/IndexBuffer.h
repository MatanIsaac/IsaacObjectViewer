/** 
 * @file IndexBuffer.h
 * @brief Header file for the IndexBuffer class.
 *  This class represents an index buffer in the graphics pipeline.
 *  It manages the storage and access of index data for rendering,
 *  And provides a method for binding and unbinding the index buffer.
 */

#pragma once

namespace isaacObjectViewer
{
    class IndexBuffer
    {
    public:
        /// @brief Constructs an IndexBuffer with the given data and count.
        /// @param data The index data.
        /// @param count The number of indices.
        IndexBuffer(const void* data, unsigned int count);

        /// @brief Destroys the IndexBuffer.
        ~IndexBuffer();

        /// @brief Binds the IndexBuffer.
        void Bind() const;

        /// @brief Unbinds the IndexBuffer.
        void Unbind() const;

        /// @brief Gets the number of indices in the IndexBuffer.
        inline unsigned int GetCount() const { return m_Count; }

    private:
        unsigned int m_RendererID;
        unsigned int m_Count;
    };
}