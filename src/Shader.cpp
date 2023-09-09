//
//  Shader.cpp
//  KFRDemo
//
//  Created by klsoline on 2023/09/04.
//

#include "Shader.h"

#include <fstream>
#include <iostream>

#include <glm/gtc/type_ptr.hpp>

Shader::Shader()
    : m_program(0)
{

}

bool Shader::Load(const std::string& vertPath, const std::string& fragPath)
{
    Unload();
    m_program = Link({Compile(GL_VERTEX_SHADER, vertPath), Compile(GL_FRAGMENT_SHADER, fragPath)});

    return IsValid();
}

bool Shader::Load(const std::string& compPath)
{
    Unload();
    m_program = Link({Compile(GL_COMPUTE_SHADER, compPath)});

    return IsValid();
}

void Shader::Unload()
{
    glDeleteProgram(m_program);
    m_program = 0;

    std::unordered_map<std::string, GLint>().swap(m_attribLocMap);
    std::unordered_map<std::string, GLint>().swap(m_uniformLocMap);
}

bool Shader::IsValid() const
{
    return glIsProgram(m_program) == GL_TRUE;
}

void Shader::Bind()
{
    glUseProgram(m_program);
}

void Shader::Unbind()
{
    glUseProgram(0);
}

void Shader::SetAttrib(const std::string& name, GLint size, GLenum type,
                       GLboolean normalized, GLsizei stride, const void* pointer)
{
    const auto loc = GetAttribLoc(name);
    glVertexAttribPointer(loc, size, type, normalized, stride, pointer);
    glEnableVertexAttribArray(loc);
}

void Shader::SetUniform(const std::string& name, bool b)
{
    glUniform1i(GetUniformLoc(name), b);
}

void Shader::SetUniform(const std::string& name, int i0)
{
    glUniform1i(GetUniformLoc(name), i0);
}

void Shader::SetUniform(const std::string& name, int i0, int i1)
{
    glUniform2i(GetUniformLoc(name), i0, i1);
}

void Shader::SetUniform(const std::string& name, int i0, int i1, int i2)
{
    glUniform3i(GetUniformLoc(name), i0, i1, i2);
}

void Shader::SetUniform(const std::string& name, int i0, int i1, int i2, int i3)
{
    glUniform4i(GetUniformLoc(name), i0, i1, i2, i3);
}

void Shader::SetUniform(const std::string& name, int components, int count, const int* iv)
{
    const auto loc = GetUniformLoc(name);

    if(components == 1)
    {
        glUniform1iv(loc, count, iv);
    }
    else if(components == 2)
    {
        glUniform2iv(loc, count, iv);
    }
    else if(components == 3)
    {
        glUniform3iv(loc, count, iv);
    }
    else if(components == 4)
    {
        glUniform4iv(loc, count, iv);
    }
}

void Shader::SetUniform(const std::string& name, float f0)
{
    glUniform1f(GetUniformLoc(name), f0);
}

void Shader::SetUniform(const std::string& name, float f0, float f1)
{
    glUniform2f(GetUniformLoc(name), f0, f1);
}

void Shader::SetUniform(const std::string& name, float f0, float f1, float f2)
{
    glUniform3f(GetUniformLoc(name), f0, f1, f2);
}

void Shader::SetUniform(const std::string& name, float f0, float f1, float f2, float f3)
{
    glUniform4f(GetUniformLoc(name), f0, f1, f2, f3);
}

void Shader::SetUniform(const std::string& name, int components, int count, const float* fv)
{
    const auto loc = GetUniformLoc(name);

    if(components == 1)
    {
        glUniform1fv(loc, count, fv);
    }
    else if(components == 2)
    {
        glUniform2fv(loc, count, fv);
    }
    else if(components == 3)
    {
        glUniform3fv(loc, count, fv);
    }
    else if(components == 4)
    {
        glUniform4fv(loc, count, fv);
    }
}

void Shader::SetUniform(const std::string& name, const glm::vec2& vec)
{
    glUniform2fv(GetUniformLoc(name), 1, glm::value_ptr(vec));
}

void Shader::SetUniform(const std::string& name, const glm::vec3& vec)
{
    glUniform3fv(GetUniformLoc(name), 1, glm::value_ptr(vec));
}

void Shader::SetUniform(const std::string& name, const glm::vec4& vec)
{
    glUniform4fv(GetUniformLoc(name), 1, glm::value_ptr(vec));
}

void Shader::SetUniform(const std::string& name, const glm::mat2& mat)
{
    glUniformMatrix2fv(GetUniformLoc(name), 1, false, glm::value_ptr(mat));
}

void Shader::SetUniform(const std::string& name, const glm::mat3& mat)
{
    glUniformMatrix3fv(GetUniformLoc(name), 1, false, glm::value_ptr(mat));
}

void Shader::SetUniform(const std::string& name, const glm::mat4& mat)
{
    glUniformMatrix4fv(GetUniformLoc(name), 1, false, glm::value_ptr(mat));
}

GLuint Shader::Compile(GLenum stage, const std::string& path)
{
    std::ifstream input(path, std::ios::in | std::ios::binary | std::ios::ate);
    if(!input)
    {
        std::cerr << "Failed to load " << path << std::endl;
        return 0;
    }

    const auto inputSize = input.tellg();
    input.seekg(0, std::ios::beg);

    const std::string source(inputSize, '\0');
    input.read(const_cast<char*>(source.data()), inputSize);
    input.close();

    const auto sourceCStr = source.c_str();

    const auto shader = glCreateShader(stage);
    glShaderSource(shader, 1, &sourceCStr, nullptr);
    glCompileShader(shader);

    GLint isCompiled = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);

    if(isCompiled != GL_TRUE)
    {
        std::cerr << "Failed to compile " << path << std::endl;

        GLint maxLength = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

        if(maxLength > 0)
        {
            std::string infoLog(maxLength - 1, '\0');
            glGetShaderInfoLog(shader, maxLength - 1, &maxLength, const_cast<char*>(infoLog.data()));
            std::cerr << infoLog << std::endl;
        }

        glDeleteShader(shader);

        return 0;
    }

    return shader;
}

GLuint Shader::Link(std::vector<GLuint>&& shaders)
{
    for(const auto& shader : shaders)
    {
        if(glIsShader(shader) != GL_TRUE)
        {
            return 0;
        }
    }

    const auto program = glCreateProgram();

    for(const auto& shader : shaders)
    {
        glAttachShader(program, shader);
    }

    glLinkProgram(program);

    GLint isLinked = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &isLinked);

    if(isLinked != GL_TRUE)
    {
        GLint maxLength = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

        if(maxLength > 0)
        {
            std::string infoLog(maxLength - 1, '\0');
            glGetProgramInfoLog(program, maxLength - 1, &maxLength, const_cast<char*>(infoLog.data()));
            std::cerr << infoLog << std::endl;
        }

        glDeleteShader(program);

        return 0;
    }

    for(const auto& shader : shaders)
    {
        glDetachShader(program, shader);
        glDeleteShader(shader);
    }

    return program;
}

GLint Shader::GetAttribLoc(const std::string& name)
{
    if(m_attribLocMap.find(name) == m_attribLocMap.end())
    {
        m_attribLocMap[name] = glGetAttribLocation(m_program, name.c_str());
    }

    return m_attribLocMap[name];
}

GLint Shader::GetUniformLoc(const std::string& name)
{
    if(m_uniformLocMap.find(name) == m_uniformLocMap.end())
    {
        m_uniformLocMap[name] = glGetUniformLocation(m_program, name.c_str());
    }

    return m_uniformLocMap[name];
}

