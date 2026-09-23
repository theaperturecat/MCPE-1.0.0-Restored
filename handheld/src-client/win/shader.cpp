#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include <iostream>
#include "shader.h"
#include <fstream>
#include <sstream>


std::string readShader(const char * filename)
{
    FILE* sfile = fopen(filename, "rb");
    if (sfile == nullptr)
    {
        return nullptr;
    }

    fseek(sfile, 0, SEEK_END);
    int length = ftell(sfile);
    fseek(sfile, 0, SEEK_SET);
    std::string fdata;
    fdata.resize(length);
    fread(fdata.data(), 1, length, sfile);
    fclose(sfile);

    return std::move(fdata);
}

unsigned int shadertypetoglconversion[] =
{
    GL_FRAGMENT_SHADER,
    GL_VERTEX_SHADER
};

ShaderComponent::ShaderComponent(const char* path, ShaderType type) : ShaderComponent(std::vector<std::string>{ "#version 330 core\n", readShader(path)}, type)
{
}

ShaderComponent::ShaderComponent(const std::vector<std::string>& chunks, ShaderType _type)
{
    type = _type;
    shaderComponent = glCreateShader(shadertypetoglconversion[_type]);

    std::vector<const char*> cc;
    for(const std::string & s : chunks)
    cc.push_back(s.c_str());

    glShaderSource(shaderComponent, cc.size(), cc.data(), NULL);
    glCompileShader(shaderComponent);

    int  success;

    glGetShaderiv(shaderComponent, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        char infoLog[2048];
        glGetShaderInfoLog(shaderComponent, 2048, NULL, infoLog);
        std::cout << "Shader error\n\n";
        std::cout << infoLog;
        exit(0);
    }
}

ShaderComponent::~ShaderComponent()
{
    glDeleteShader(shaderComponent);
}

Shader::Shader(const ShaderComponent& vertex, const ShaderComponent& fragment)
{
    if (vertex.type != VERTEX_SHADER || fragment.type != FRAGMENT_SHADER)
    {
        std::cout << "Wrong shader parts passed\n";
        return;
    }

    id = glCreateProgram();
    glAttachShader(id, vertex.shaderComponent);
    glAttachShader(id, fragment.shaderComponent);
    glLinkProgram(id);

    int  success;
    glGetProgramiv(id, GL_LINK_STATUS, &success);

    if (!success)
    {
        char infoLog[2048];
        glGetShaderInfoLog(id, 2048, NULL, infoLog);
        std::cout << "Shader error\n\n";
        std::cout << infoLog;
        exit(0);
    }
}

Shader::~Shader()
{
    glDeleteProgram(id);
}

void Shader::activate() const 
{
    glUseProgram(id);
}

void Shader::setMatrix2(const std::string& name, float* value) const
{
    activate();
    unsigned int projLoc = glGetUniformLocation(id, name.c_str());
    glUniformMatrix2fv(projLoc, 1, GL_FALSE, value);
}

void Shader::setMatrix3(const std::string& name, float* value) const
{
    activate();
    unsigned int projLoc = glGetUniformLocation(id, name.c_str());
    glUniformMatrix3fv(projLoc, 1, GL_FALSE, value);
}

void Shader::setMatrix4(const std::string& name, float* value) const
{
    activate();
    unsigned int projLoc = glGetUniformLocation(id, name.c_str());
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, value);
}

void Shader::setBool(const std::string& name, bool value) const
{
    activate();
     glUniform1i(glGetUniformLocation(id, name.c_str()), value);
}

void Shader::setInt(const std::string& name, int value) const
{
    activate();
    glUniform1i(glGetUniformLocation(id, name.c_str()), value);
}

void Shader::setFloat(const std::string& name, float value) const
{
    activate();
    glUniform1f(glGetUniformLocation(id, name.c_str()), value);
}

void Shader::setVector2(const std::string& name, float x, float y) const
{
    activate();
    glUniform2f(glGetUniformLocation(id, name.c_str()), x, y);
}

void Shader::setVector2(const std::string& name, glm::vec2& vec) const
{
    activate();
    glUniform2f(glGetUniformLocation(id, name.c_str()), vec.x, vec.y);
}

void Shader::setVector3(const std::string& name, float x,float y,float z) const
{
    activate();
    glUniform3f(glGetUniformLocation(id, name.c_str()), x,y,z);
}

void Shader::setVector3(const std::string& name, glm::vec3& vec) const
{
    activate();
    glUniform3f(glGetUniformLocation(id, name.c_str()), vec.x, vec.y, vec.z);
}

void Shader::setVector4(const std::string& name, float x, float y, float z, float w) const
{
    activate();
    glUniform4f(glGetUniformLocation(id, name.c_str()), x, y, z, w);
}

void Shader::setVector4(const std::string& name, glm::vec4& vec) const
{
    activate();
    glUniform4f(glGetUniformLocation(id, name.c_str()), vec.x, vec.y,vec.z,vec.w);
}

void Shader::setMatrix2(unsigned int offset, float* value) const
{
    activate();
    glUniformMatrix2fv(offset, 1, GL_FALSE, value);
}

void Shader::setMatrix3(unsigned int offset, float* value) const
{
    activate();
    glUniformMatrix3fv(offset, 1, GL_FALSE, value);
}

void Shader::setMatrix4(unsigned int offset, float* value) const
{
    activate();
    glUniformMatrix4fv(offset, 1, GL_FALSE, value);
}

void Shader::setBool(unsigned int offset, bool value) const
{
    activate();
    glUniform1i(offset, value);
}

void Shader::setInt(unsigned int offset, int value) const
{
    activate();
    glUniform1i(offset, value);
}

void Shader::setFloat(unsigned int offset, float value) const
{
    activate();
    glUniform1f(offset, value);
}

void Shader::setVector2(unsigned int offset, float x, float y) const
{
    activate();
    glUniform2f(offset, x, y);
}

void Shader::setVector2(unsigned int offset, glm::vec2& vec) const
{
    activate();
    glUniform2f(offset, vec.x, vec.y);
}

void Shader::setVector3(unsigned int offset, float x, float y, float z) const
{
    activate();
    glUniform3f(offset, x, y, z);
}

void Shader::setVector3(unsigned int offset, glm::vec3& vec) const
{
    activate();
    glUniform3f(offset, vec.x, vec.y, vec.z);
}

void Shader::setVector4(unsigned int offset, float x, float y, float z, float w) const
{
    activate();
    glUniform4f(offset, x, y, z, w);
}

void Shader::setVector4(unsigned int offset, glm::vec4& vec) const
{
    activate();
    glUniform4f(offset, vec.x, vec.y, vec.z, vec.w);
}