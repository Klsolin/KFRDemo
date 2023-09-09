///////////////////////////////////////////////////////////////////////////////
// FrameBuffer.h
// =============
// class for OpenGL Frame Buffer Object (FBO)
// It contains a 32bit color buffer and a depth buffer as GL_DEPTH_COMPONENT24
// Call init() to create/resize a FBO with given width and height params.
// It supports MSAA (Multi Sample Anti Aliasing) FBO. If msaa=0, it creates a
// single-sampled FBO. If msaa > 0 (even number), it creates a multi-sampled
// FBO.
//
// NOTE: This class does not use GL_ARB_texture_multisample extension yet. Call
//       update() function explicitly to blit color/depth buffers from
//       multi-sample FBO to single-sample FBO if you want to get single-sample
//       color and depth data from MSAA FBO.
//
//  AUTHOR: Song Ho Ahn (song.ahn@gmail.com)
// CREATED: 2015-03-05
// UPDATED: 2020-10-03
///////////////////////////////////////////////////////////////////////////////

#pragma once

//@@ define missing color formats in macOS
#ifndef GL_RGBA32F
#define GL_RGBA32F 0x8814
#endif
#ifndef GL_RGB32F
#define GL_RGB32F  0x8815
#endif
#ifndef GL_RGBA16F
#define GL_RGBA16F 0x881A
#endif
#ifndef GL_RGB16F
#define GL_RGB16F  0x881B
#endif

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 1
#endif
#include <windows.h>
#endif

#include <string>

#include <glad/glad.h>

class FrameBuffer
{
public:
    FrameBuffer();

    bool Init(int width, int height, int msaa=0);
    void Destroy();

    void Update();                                  // copy multi-sample to single-sample and generate mipmaps

    void BlitColorTo(FrameBuffer& fb);              // blit color to another FrameBuffer instance
    void BlitColorTo(GLuint fboId, int x=0, int y=0, int w=0, int h=0); // copy color buffer using FBO ID
    void BlitDepthTo(FrameBuffer& fb);              // blit depth to another FrameBuffer instance
    void BlitDepthTo(GLuint fboId, int x=0, int y=0, int w=0, int h=0); // copy depth buffer using FBO ID

    void CopyColorBuffer();                         // copy color to array
    void CopyDepthBuffer();                         // copy depth to array
    const unsigned char* GetColorBuffer() const     { return colorBuffer; }
    const float* GetDepthBuffer() const             { return depthBuffer; }

    GLuint GetID() const;
    GLuint GetTexID() const                       { return texId; }   // single-sample texture object
    GLuint GetDepthID() const                       { return rboId; }   // single-sample rbo

    int GetWidth() const                            { return width; }
    int GetHeight() const                           { return height; }
    int GetMsaa() const                             { return msaa; }
    std::string GetStatus() const;                  // return FBO info
    std::string GetErrorMessage() const             { return errorMessage; }

    bool IsValid();

    void Bind();
    void Unbind();

private:
    static std::string GetTextureParameters(GLuint id);
    static std::string GetRenderbufferParameters(GLuint id);
    static std::string ConvertInternalFormatToString(GLenum format);

    // member vars
    int width;                      // buffer width
    int height;                     // buffer height
    int msaa;                       // # of multi samples; 0, 2, 4, 8,...
    unsigned char* colorBuffer;     // color buffer (rgba)
    float* depthBuffer;             // depth buffer
    GLuint fboMsaaId;               // primary id for multisample FBO
    GLuint rboMsaaColorId;          // id for multisample RBO (color buffer)
    GLuint rboMsaaDepthId;          // id for multisample RBO (depth buffer)
    GLuint fboId;                   // secondary id for frame buffer object
    GLuint texId;                   // id for texture object (color buffer)
    GLuint rboId;                   // id for render buffer object (depth buffer)
    std::string errorMessage;
};

