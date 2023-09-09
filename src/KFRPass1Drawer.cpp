//
//  KFRPass1Drawer.cpp
//  KFRDemo
//
//  Created by klsoline on 2023/09/04.
//

#include "KFRPass1Drawer.h"

#include <algorithm>

#include "Texture.h"

KFRPass1Drawer::KFRPass1Drawer()
    : m_param{1.0f, 1.0f}
{

}

bool KFRPass1Drawer::Init(const Shader& shader)
{
    if(!MeshDrawer::Init(shader))
    {
        return false;
    }

    m_shader.Bind();
    m_shader.SetUniform("sPosNormalXTex", 0);
    m_shader.SetUniform("sSpecMatNormalYTex", 1);
    m_shader.SetUniform("sDiffMapShininessMatTex", 2);
    m_shader.Unbind();

    return true;
}

void KFRPass1Drawer::Destroy()
{
    m_outGBuf.Destroy();
}

void KFRPass1Drawer::Draw()
{
    if(!m_inGBuf.IsValid() || !m_outGBuf.IsValid())
    {
        return;
    }

    m_outGBuf.Bind();

    glViewport(0, 0, m_outGBuf.GetWidth(), m_outGBuf.GetHeight());
    glClear(GL_COLOR_BUFFER_BIT);

    Texture::TexBind(m_inGBuf.GetPosNormalXTexID(), 0);
    Texture::TexBind(m_inGBuf.GetSpecMatNormalYTexID(), 1);
    Texture::TexBind(m_inGBuf.GetDiffMapShininessMatTexID(), 2);

    MeshDrawer::Draw();

    Texture::TexUnbind(2);
    Texture::TexUnbind(1);
    Texture::TexUnbind(0);

    m_outGBuf.Unbind();
}

void KFRPass1Drawer::Setup(const GBuffer& gbuf)
{
    m_inGBuf = gbuf;

    m_outGBuf.Init(std::max(1, int(m_inGBuf.GetWidth() / m_param.downscaleRatio)),
                   std::max(1, int(m_inGBuf.GetHeight() / m_param.downscaleRatio)));
}

void KFRPass1Drawer::Setup(const KFRParam& param)
{
    m_param = param;

    m_outGBuf.Init(std::max(1, int(m_inGBuf.GetWidth() / m_param.downscaleRatio)),
                   std::max(1, int(m_inGBuf.GetHeight() / m_param.downscaleRatio)));

    m_shader.Bind();
    m_shader.SetUniform("uParam.kernelParam", m_param.kernelParam);
    m_shader.SetUniform("uParam.downscaleRatio", m_param.downscaleRatio);
    m_shader.Unbind();
}

void KFRPass1Drawer::Setup(int gazePointX, int gazePointY)
{
    m_shader.Bind();
    m_shader.SetUniform("uGazePoint", float(gazePointX), float(gazePointY));
    m_shader.Unbind();
}

void KFRPass1Drawer::Resize(int width, int height)
{
    m_shader.Bind();
    m_shader.SetUniform("uRes", float(width), float(height));
    m_shader.Unbind();
}

GBuffer KFRPass1Drawer::Output() const
{
    return m_outGBuf;
}

