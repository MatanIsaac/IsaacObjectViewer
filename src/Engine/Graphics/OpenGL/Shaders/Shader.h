#pragma once

#include "Utility/config.h"
#include "Utility/GLErrorManager.h"

namespace isaacObjectViewer
{

class Shader
{
public:
    bool isBinded;
    unsigned int ID;
    // constructor generates the shader on the fly
    Shader(const char* vertexPath, const char* fragmentPath);

    // bind/unbind the shader
    void Bind() const;

    void Unbind() const;

    // utility uniform functions
    void setBool(const std::string& name, bool value) const;

    void setInt(const std::string& name, int value) const;

    void setFloat(const std::string& name, float value) const;

    void setVec2(const std::string& name, const glm::vec2& value) const;

    void setVec2(const std::string& name, float x, float y) const;

    void setVec3(const std::string& name, const glm::vec3& value) const;

    void setVec3(const std::string& name, float x, float y, float z) const;

    void setVec4(const std::string& name, const glm::vec4& value) const;

    void setVec4(const std::string& name, float x, float y, float z, float w) const;

    void setMat2(const std::string& name, const glm::mat2& mat) const;

    void setMat3(const std::string& name, const glm::mat3& mat) const;

    void setMat4(const std::string& name, const glm::mat4& mat) const;

private:
    // utility function for checking shader compilation/linking errors.
    // ------------------------------------------------------------------------
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