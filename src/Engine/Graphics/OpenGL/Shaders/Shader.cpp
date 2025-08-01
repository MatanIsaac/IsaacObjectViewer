#include "Shader.h"
#include "Utility/Log.hpp"

namespace isaacObjectViewer
{

    Shader::Shader(const char *vertexPath, const char *fragmentPath)
    {
        // 1. retrieve the vertex/fragment source code from filePath
        std::string vertexCode;
        std::string fragmentCode;
        std::ifstream vShaderFile;
        std::ifstream fShaderFile;
        // ensure ifstream objects can throw exceptions:
        vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try
        {
            // open files
            vShaderFile.open(vertexPath);
            fShaderFile.open(fragmentPath);
            std::stringstream vShaderStream, fShaderStream;
            // read file's buffer contents into streams
            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();
            // close file handlers
            vShaderFile.close();
            fShaderFile.close();
            // convert stream into string
            vertexCode = vShaderStream.str();
            fragmentCode = fShaderStream.str();
        }
        catch (std::ifstream::failure &e)
        {
            LOG_ERROR("ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: {}", e.what());
        }
        const char *vShaderCode = vertexCode.c_str();
        const char *fShaderCode = fragmentCode.c_str();
        // 2. compile shaders
        unsigned int vertex, fragment;
        // vertex shader
        vertex = glCreateShader(GL_VERTEX_SHADER);
        GLCall(glShaderSource(vertex, 1, &vShaderCode, NULL));
        GLCall(glCompileShader(vertex));
        checkCompileErrors(vertex, "VERTEX");
        // fragment Shader
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        GLCall(glShaderSource(fragment, 1, &fShaderCode, NULL));
        GLCall(glCompileShader(fragment));
        checkCompileErrors(fragment, "FRAGMENT");
        // shader Program
        ID = glCreateProgram();
        GLCall(glAttachShader(ID, vertex));
        GLCall(glAttachShader(ID, fragment));
        GLCall(glLinkProgram(ID));
        checkCompileErrors(ID, "PROGRAM");
        // delete the shaders as they're linked into our program now and no longer necessary
        GLCall(glDeleteShader(vertex));
        GLCall(glDeleteShader(fragment));
    }

    void Shader::Bind() const 
    { GLCall(glUseProgram(ID)); }

    void Shader::Unbind() const { GLCall(glUseProgram(0)); }

    void Shader::setBool(const std::string &name, bool value) const
    {
        GLCall(glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value));
    }

    void Shader::setInt(const std::string &name, int value) const
    {
        GLCall(glUniform1i(glGetUniformLocation(ID, name.c_str()), value));
    }

    void Shader::setFloat(const std::string &name, float value) const
    {
        GLCall(glUniform1f(glGetUniformLocation(ID, name.c_str()), value));
    }

    void Shader::setVec2(const std::string &name, const glm::vec2 &value) const
    {
        GLCall(glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]));
    }
    void Shader::setVec2(const std::string &name, float x, float y) const
    {
        GLCall(glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y));
    }

    void Shader::setVec3(const std::string &name, const glm::vec3 &value) const
    {
        GLCall(glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]));
    }
    void Shader::setVec3(const std::string &name, float x, float y, float z) const
    {
        GLCall(glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z));
    }

    void Shader::setVec4(const std::string &name, const glm::vec4 &value) const
    {
        GLCall(glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]));
    }
    void Shader::setVec4(const std::string &name, float x, float y, float z, float w) const
    {
        GLCall(glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w));
    }

    void Shader::setMat2(const std::string &name, const glm::mat2 &mat) const
    {
        GLCall(glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]));
    }

    void Shader::setMat3(const std::string &name, const glm::mat3 &mat) const
    {
        GLCall(glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]));
    }

    void Shader::setMat4(const std::string &name, const glm::mat4 &mat) const
    {
        GLCall(glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]));
    }

} // namespace isaacGraphicsEngine