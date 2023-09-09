//
//  Texture2DDrawer.cpp
//  KFRDemo
//
//  Created by klsoline on 2023/09/04.
//

#include "Texture2DDrawer.h"

Texture2DDrawer::Texture2DDrawer()
    : m_texID(0), m_hasAlpha(false)
    , m_resX(1), m_resY(1)
{

}

void Texture2DDrawer::Draw()
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

void Texture2DDrawer::DrawFullScreen()
{
    Setup(m_resX * 0.5f, m_resY * 0.5f, float(m_resX), float(m_resY));
    Draw();
}

void Texture2DDrawer::Setup(const Texture& tex)
{
    Setup(tex.GetTexID(), tex.GetChannels() == 4);
}

void Texture2DDrawer::Setup(GLuint texID, bool hasAlpha/* = false */)
{
    m_texID = texID;
    m_hasAlpha = hasAlpha;
}

void Texture2DDrawer::Setup(float x, float y, float w, float h)
{
    m_shader.Bind();
    m_shader.SetUniform("uPos", x, y);
    m_shader.SetUniform("uSize", w, h);
    m_shader.Unbind();
}

void Texture2DDrawer::Resize(int width, int height)
{
    m_resX = width;
    m_resY = height;

    m_shader.Bind();
    m_shader.SetUniform("uRes", float(m_resX), float(m_resY));
    m_shader.Unbind();
}

