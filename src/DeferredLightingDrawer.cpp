//
//  DeferredLightingDrawer.cpp
//  KFRDemo
//
//  Created by klsoline on 2023/09/04.
//

#include "DeferredLightingDrawer.h"

#include "Texture.h"

bool DeferredLightingDrawer::Init(const Shader& shader)
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

void DeferredLightingDrawer::Destroy()
{
    m_outFbo.Destroy();
}

void DeferredLightingDrawer::Draw()
{
    if(!m_inGBuf.IsValid() || !m_outFbo.IsValid())
    {
        return;
    }

    m_outFbo.Bind();

    glViewport(0, 0, m_outFbo.GetWidth(), m_outFbo.GetHeight());
    glClear(GL_COLOR_BUFFER_BIT);

    Texture::TexBind(m_inGBuf.GetPosNormalXTexID(), 0);
    Texture::TexBind(m_inGBuf.GetSpecMatNormalYTexID(), 1);
    Texture::TexBind(m_inGBuf.GetDiffMapShininessMatTexID(), 2);

    MeshDrawer::Draw();

    Texture::TexUnbind(2);
    Texture::TexUnbind(1);
    Texture::TexUnbind(0);

    m_outFbo.Unbind();
}

void DeferredLightingDrawer::Setup(const GBuffer& gbuf)
{
    m_inGBuf = gbuf;
    m_outFbo.Init(m_inGBuf.GetWidth(), m_inGBuf.GetHeight());
}

void DeferredLightingDrawer::Setup(const glm::vec3& eyePos)
{
    m_shader.Bind();
    m_shader.SetUniform("uEyePos", eyePos);
    m_shader.Unbind();
}

void DeferredLightingDrawer::Setup(const std::vector<Light>& lights, int lightCount)
{
    m_shader.Bind();

    m_shader.SetUniform("uLightCount", lightCount);

    for(int i = 0; i < lightCount; ++i)
    {
        const std::string baseName = "uLights[" + std::to_string(i) + "]";

        m_shader.SetUniform(baseName + ".pos", lights[i].pos);
        // m_shader.SetUniform(baseName + ".dir", lights[i].dir);
        m_shader.SetUniform(baseName + ".La", lights[i].La);
        m_shader.SetUniform(baseName + ".Ld", lights[i].Ld);
        m_shader.SetUniform(baseName + ".Ls", lights[i].Ls);
        m_shader.SetUniform(baseName + ".att", lights[i].att);
    }

    m_shader.Unbind();
}

FrameBuffer DeferredLightingDrawer::Output() const
{
    return m_outFbo;
}

