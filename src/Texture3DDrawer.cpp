//
//  Texture3DDrawer.cpp
//  KFRDemo
//
//  Created by klsoline on 2023/09/04.
//

#include "Texture3DDrawer.h"

Texture3DDrawer::Texture3DDrawer()
    : m_texID(0), m_hasAlpha(false)
{

}

void Texture3DDrawer::Draw()
{
    if(!Texture::IsTexValid(m_texID))
    {
        return;
    }

    if(m_hasAlpha)
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
    else
    {
        glDisable(GL_BLEND);
    }

    Texture::TexBind(m_texID);
    MeshDrawer::Draw();
    Texture::TexUnbind();

    glDisable(GL_BLEND);
}

void Texture3DDrawer::Setup(const Texture& tex)
{
    Setup(tex.GetTexID(), tex.GetChannels() == 4);
}

void Texture3DDrawer::Setup(GLuint texID, bool hasAlpha/* = false */)
{
    m_texID = texID;
    m_hasAlpha = hasAlpha;
}

void Texture3DDrawer::Setup(const glm::mat4& modelMatrix, const glm::mat4& viewMatrix, const glm::mat4& projMatrix)
{
    m_shader.Bind();
    m_shader.SetUniform("uModelMatrix", modelMatrix);
    m_shader.SetUniform("uViewMatrix", viewMatrix);
    m_shader.SetUniform("uProjMatrix", projMatrix);
    m_shader.Unbind();
}

void Texture3DDrawer::UpdateVao()
{
    glBindVertexArray(m_vao);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);

    m_shader.SetAttrib("aPos", 3, GL_FLOAT, GL_FALSE, m_attrSize * sizeof(float), (const void*)0);
    m_shader.SetAttrib("aTexCoord", 2, GL_FLOAT, GL_FALSE, m_attrSize * sizeof(float), (const void*)(3 * sizeof(float)));
    m_shader.SetAttrib("aNormal", 3, GL_FLOAT, GL_FALSE, m_attrSize * sizeof(float), (const void*)(5 * sizeof(float)));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

