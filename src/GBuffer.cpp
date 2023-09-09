//
//  GBuffer.cpp
//  KFRDemo
//
//  Created by klsoline on 2023/09/04.
//

#include "GBuffer.h"

GBuffer::GBuffer()
    : m_width(0), m_height(0)
    , m_gBufferID(0)
    , m_posNormalXTexID(0)
    , m_specMatNormalYTexID(0)
    , m_diffMapShininessMatTexID(0)
    , m_depthRenderBufferID(0)
{

}

bool GBuffer::Init(int width, int height)
{
    if(!(width > 0 && height > 0))
    {
        return false;
    }

    if(IsValid() && (width == m_width && height == m_height))
    {
        return true;
    }

    Destroy();

    m_width = width;
    m_height = height;

    glGenFramebuffers(1, &m_gBufferID);
    glBindFramebuffer(GL_FRAMEBUFFER, m_gBufferID);

    glGenTextures(1, &m_posNormalXTexID);
    glBindTexture(GL_TEXTURE_2D, m_posNormalXTexID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_width, m_height, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_posNormalXTexID, 0);

    glGenTextures(1, &m_specMatNormalYTexID);
    glBindTexture(GL_TEXTURE_2D, m_specMatNormalYTexID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_width, m_height, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_specMatNormalYTexID, 0);

    glGenTextures(1, &m_diffMapShininessMatTexID);
    glBindTexture(GL_TEXTURE_2D, m_diffMapShininessMatTexID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, m_diffMapShininessMatTexID, 0);

    glGenRenderbuffers(1, &m_depthRenderBufferID);
    glBindRenderbuffer(GL_RENDERBUFFER, m_depthRenderBufferID);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_width, m_height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthRenderBufferID);

    GLuint attachments[] =
    {
        GL_COLOR_ATTACHMENT0,
        GL_COLOR_ATTACHMENT1,
        GL_COLOR_ATTACHMENT2
    };

    glDrawBuffers(sizeof(attachments) / sizeof(attachments[0]), attachments);

    return IsValid();
}

void GBuffer::Destroy()
{
    glDeleteFramebuffers(1, &m_gBufferID);

    glDeleteTextures(1, &m_posNormalXTexID);
    glDeleteTextures(1, &m_specMatNormalYTexID);
    glDeleteTextures(1, &m_diffMapShininessMatTexID);

    glDeleteRenderbuffers(1, &m_depthRenderBufferID);

    m_width = m_height = 0;

    m_gBufferID = 0;

    m_posNormalXTexID = 0;
    m_specMatNormalYTexID = 0;
    m_diffMapShininessMatTexID = 0;

    m_depthRenderBufferID = 0;
}

int GBuffer::GetWidth() const
{
    return m_width;
}

int GBuffer::GetHeight() const
{
    return m_height;
}

GLuint GBuffer::GetPosNormalXTexID() const
{
    return m_posNormalXTexID;
}

GLuint GBuffer::GetSpecMatNormalYTexID() const
{
    return m_specMatNormalYTexID;
}

GLuint GBuffer::GetDiffMapShininessMatTexID() const
{
    return m_diffMapShininessMatTexID;
}

bool GBuffer::IsValid() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_gBufferID);
    const bool ret = (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return ret;
}

void GBuffer::Bind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_gBufferID);
}

void GBuffer::Unbind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

