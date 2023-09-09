//
//  Shader.h
//  KFRDemo
//
//  Created by klsoline on 2023/09/04.
//

#pragma once

#include <vector>
#include <string>
#include <unordered_map>

#include <glad/glad.h>
#include <glm/glm.hpp>

class Shader
{
public:
    Shader();

    bool Load(const std::string& vertPath, const std::string& fragPath);
    bool Load(const std::string& compPath);

    void Unload();

    bool IsValid() const;

    void Bind();
    void Unbind();

    void SetAttrib(const std::string& name, GLint size, GLenum type,
                   GLboolean normalized, GLsizei stride, const void* pointer);

    void SetUniform(const std::string& name, bool b);

    void SetUniform(const std::string& name, int i0);
    void SetUniform(const std::string& name, int i0, int i1);
    void SetUniform(const std::string& name, int i0, int i1, int i2);
    void SetUniform(const std::string& name, int i0, int i1, int i2, int i3);
    void SetUniform(const std::string& name, int components, int count, const int* iv);

    void SetUniform(const std::string& name, float f0);
    void SetUniform(const std::string& name, float f0, float f1);
    void SetUniform(const std::string& name, float f0, float f1, float f2);
    void SetUniform(const std::string& name, float f0, float f1, float f2, float f3);
    void SetUniform(const std::string& name, int components, int count, const float* fv);

    void SetUniform(const std::string& name, const glm::vec2& vec);
    void SetUniform(const std::string& name, const glm::vec3& vec);
    void SetUniform(const std::string& name, const glm::vec4& vec);

    void SetUniform(const std::string& name, const glm::mat2& mat);
    void SetUniform(const std::string& name, const glm::mat3& mat);
    void SetUniform(const std::string& name, const glm::mat4& mat);

private:
    GLuint m_program;

    std::unordered_map<std::string, GLint> m_attribLocMap;
    std::unordered_map<std::string, GLint> m_uniformLocMap;

    GLuint Compile(GLenum stage, const std::string& path);
    GLuint Link(std::vector<GLuint>&& shaders);

    GLint GetAttribLoc(const std::string& name);
    GLint GetUniformLoc(const std::string& name);
};

