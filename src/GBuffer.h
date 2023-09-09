//
//  GBuffer.h
//  KFRDemo
//
//  Created by klsoline on 2023/09/04.
//

#pragma once

#include <glad/glad.h>

class GBuffer
{
public:
    GBuffer();

    bool Init(int width, int height);
    void Destroy();

    int GetWidth() const;
    int GetHeight() const;

    GLuint GetPosNormalXTexID() const;
    GLuint GetSpecMatNormalYTexID() const;
    GLuint GetDiffMapShininessMatTexID() const;

    bool IsValid() const;

    void Bind();
    void Unbind();

private:
    int m_width, m_height;

    GLuint m_gBufferID;

    GLuint m_posNormalXTexID;
    GLuint m_specMatNormalYTexID;
    GLuint m_diffMapShininessMatTexID;

    GLuint m_depthRenderBufferID;
};

