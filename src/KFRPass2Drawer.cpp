//
//  KFRPass2Drawer.cpp
//  KFRDemo
//
//  Created by klsoline on 2023/09/04.
//

#include "KFRPass2Drawer.h"

#include <algorithm>

#include "Texture.h"

KFRPass2Drawer::KFRPass2Drawer()
    : m_param{1.0f, 1.0f}
{

}

void KFRPass2Drawer::Destroy()
{
    m_outFbo.Destroy();
}

void KFRPass2Drawer::Draw()
{
    if(!m_inFbo.IsValid() || !m_outFbo.IsValid())
    {
        return;
    }

    m_outFbo.Bind();

    glViewport(0, 0, m_outFbo.GetWidth(), m_outFbo.GetHeight());
    glClear(GL_COLOR_BUFFER_BIT);

    Texture::TexBind(m_inFbo.GetTexID());
    MeshDrawer::Draw();
    Texture::TexUnbind();

    m_outFbo.Unbind();
}

void KFRPass2Drawer::Setup(const FrameBuffer& fbo)
{
    m_inFbo = fbo;

    m_outFbo.Init(std::max(1, int(m_inFbo.GetWidth() * m_param.downscaleRatio)),
                  std::max(1, int(m_inFbo.GetHeight() * m_param.downscaleRatio)));
}

void KFRPass2Drawer::Setup(const KFRParam& param)
{
    m_param = param;

    m_outFbo.Init(std::max(1, int(m_inFbo.GetWidth() * m_param.downscaleRatio)),
                  std::max(1, int(m_inFbo.GetHeight() * m_param.downscaleRatio)));

    m_shader.Bind();
    m_shader.SetUniform("uParam.kernelParam", param.kernelParam);
    m_shader.SetUniform("uParam.downscaleRatio", param.downscaleRatio);
    m_shader.Unbind();
}

void KFRPass2Drawer::Setup(int gazePointX, int gazePointY)
{
    m_shader.Bind();
    m_shader.SetUniform("uGazePoint", float(gazePointX), float(gazePointY));
    m_shader.Unbind();
}

void KFRPass2Drawer::Resize(int width, int height)
{
    m_shader.Bind();
    m_shader.SetUniform("uRes", float(width), float(height));
    m_shader.Unbind();
}

FrameBuffer KFRPass2Drawer::Output() const
{
    return m_outFbo;
}

