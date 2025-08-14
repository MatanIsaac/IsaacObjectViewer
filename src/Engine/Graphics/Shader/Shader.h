/**
 * @file Shader.h
 * @brief Header file for the Shader class.
 * This class is responsible for managing and using shaders in the rendering pipeline.
 */

#pragma once

#include "Utility/config.h"
#include "Utility/GLErrorManager.h"

namespace isaacObjectViewer
{

class Shader
{
public:
    /// @brief Indicates whether the shader is currently bound.
    bool isBinded;
    /// @brief The ID of the shader program.
    unsigned int ID;

    /// @brief Constructs a shader program from vertex and fragment shader source files.
    /// @param vertexPath The file path to the vertex shader source.
    /// @param fragmentPath The file path to the fragment shader source.
    Shader(const char* vertexPath, const char* fragmentPath);

    /// @brief Binds the shader program.
    void Bind() const;

    /// @brief Unbinds the shader program.
    void Unbind() const;

    /// @brief Sets a boolean uniform variable in the shader.
    /// @param name The name of the uniform variable.
    /// @param value The value to set.
    void setBool(const std::string& name, bool value) const;

    /// @brief Sets an integer uniform variable in the shader.
    /// @param name The name of the uniform variable.
    /// @param value The value to set.
    void setInt(const std::string& name, int value) const;

    /// @brief Sets a float uniform variable in the shader.
    /// @param name The name of the uniform variable.
    /// @param value The value to set.
    void setFloat(const std::string& name, float value) const;

    /// @brief Sets a 2D vector uniform variable in the shader.
    /// @param name The name of the uniform variable.
    /// @param value The value to set.
    void setVec2(const std::string& name, const glm::vec2& value) const;

    /// @brief Sets a 2D vector uniform variable in the shader.
    /// @param name The name of the uniform variable.
    /// @param x The x component of the vector.
    /// @param y The y component of the vector.
    void setVec2(const std::string& name, float x, float y) const;

    /// @brief Sets a 3D vector uniform variable in the shader.
    /// @param name The name of the uniform variable.
    /// @param value The value to set.
    void setVec3(const std::string& name, const glm::vec3& value) const;

    /// @brief Sets a 3D vector uniform variable in the shader.
    /// @param name The name of the uniform variable.
    /// @param x The x component of the vector.
    /// @param y The y component of the vector.
    /// @param z The z component of the vector.
    void setVec3(const std::string& name, float x, float y, float z) const;

    /// @brief Sets a 4D vector uniform variable in the shader.
    /// @param name The name of the uniform variable.
    /// @param value The value to set.
    void setVec4(const std::string& name, const glm::vec4& value) const;

    /// @brief Sets a 4D vector uniform variable in the shader.
    /// @param name The name of the uniform variable.
    /// @param x The x component of the vector.
    /// @param y The y component of the vector.
    /// @param z The z component of the vector.
    /// @param w The w component of the vector.
    void setVec4(const std::string& name, float x, float y, float z, float w) const;

    /// @brief Sets a 2x2 matrix uniform variable in the shader.
    /// @param name The name of the uniform variable.
    /// @param mat The 2x2 matrix to set.
    void setMat2(const std::string& name, const glm::mat2& mat) const;

    /// @brief Sets a 3x3 matrix uniform variable in the shader.
    /// @param name The name of the uniform variable.
    /// @param mat The 3x3 matrix to set.
    void setMat3(const std::string& name, const glm::mat3& mat) const;

    /// @brief Sets a 4x4 matrix uniform variable in the shader.
    /// @param name The name of the uniform variable.
    /// @param mat The 4x4 matrix to set.
    void setMat4(const std::string& name, const glm::mat4& mat) const;

private:
    /// @brief Checks for shader compilation and linking errors
    /// @param shader The shader ID.
    /// @param type The type of shader (vertex, fragment, etc.).
    void checkCompileErrors(GLuint shader, std::string type)
    {
        GLint  success;
        GLchar infoLog[1024];
        if (type != "PROGRAM")
        {
            GLCall(glGetShaderiv(shader, GL_COMPILE_STATUS, &success));
            if (!success)
            {
                GLCall(glGetShaderInfoLog(shader, 1024, NULL, infoLog));
                LOG_ERROR("ERROR::SHADER_COMPILATION_ERROR of type: {}\n{}\n---------------------------------------------------------",type,infoLog);
            }
        }
        else
        {
            GLCall(glGetProgramiv(shader, GL_LINK_STATUS, &success));
            if (!success)
            {
                GLCall(glGetProgramInfoLog(shader, 1024, NULL, infoLog));
                LOG_ERROR("ERROR::PROGRAM_LINKING_ERROR of type: {}\n{}\n---------------------------------------------------------",type,infoLog);
            }
        }
    }
};
}  // namespace isaacGraphicsEngine