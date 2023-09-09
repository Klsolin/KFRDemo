//
//  MeshDrawer.h
//  KFRDemo
//
//  Created by klsoline on 2023/09/04.
//

#pragma once

#include "Shader.h"
#include "Mesh.h"

class MeshDrawer
{
public:
    MeshDrawer();
    virtual ~MeshDrawer();

    virtual bool Init(const Shader& shader);
    virtual void Destroy();
    virtual void Draw();
    virtual void Update(const Mesh& mesh, bool exactFit = false);

    bool IsValid() const;

    Shader GetShader() const;

protected:
    Shader m_shader;

    GLenum m_drawMode;

    int m_attrSize;
    std::string m_attrDesc;

    GLuint m_vao, m_vbo, m_ibo;
    int m_totalVboSize, m_currVboSize;
    int m_totalIboSize, m_currIboSize;

    virtual void UpdateVao();
};

