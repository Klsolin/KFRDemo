//
//  Texture.cpp
//  KFRDemo
//
//  Created by klsoline on 2023/09/04.
//

#include "Texture.h"

#include <cstdint>

#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

Texture::Texture()
    : m_texID(0)
    , m_width(0), m_height(0), m_channels(0)
{

}

bool Texture::Load(const std::string& texPath, bool genMipmaps/* = false */)
{
    Unload();

    stbi_set_flip_vertically_on_load(true);

    const auto data = stbi_load(texPath.c_str(), &m_width, &m_height, &m_channels, 0);

    if(!data)
    {
        std::cerr << "Failed to load " << texPath << std::endl;
        return false;
    }

    glGenTextures(1, &m_texID);
    glBindTexture(GL_TEXTURE_2D, m_texID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, genMipmaps? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    const GLenum format = ((m_channels == 4)? GL_RGBA : GL_RGB);
    glTexImage2D(GL_TEXTURE_2D, 0, format, m_width, m_height, 0, format, GL_UNSIGNED_BYTE, data);
    stbi_image_free(data);

    if(genMipmaps)
    {
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    glBindTexture(GL_TEXTURE_2D, 0);

    return IsValid();
}

void Texture::Unload()
{
    glDeleteTextures(1, &m_texID);

    m_texID = 0;
    m_width = m_height = m_channels = 0;
}

GLuint Texture::GetTexID() const
{
    return m_texID;
}

int Texture::GetWidth() const
{
    return m_width;
}

int Texture::GetHeight() const
{
    return m_height;
}

int Texture::GetChannels() const
{
    return m_channels;
}

bool Texture::IsValid() const
{
    return IsTexValid(m_texID);
}

void Texture::Bind(int texUnit/* = 0 */)
{
    TexBind(m_texID, texUnit);
}

void Texture::Unbind(int texUnit/* = 0 */)
{
    TexUnbind(texUnit);
}

bool Texture::IsTexValid(GLuint texID)
{
    return glIsTexture(texID) == GL_TRUE;
}

void Texture::TexBind(GLuint texID, int texUnit/* = 0 */)
{
    glActiveTexture(GL_TEXTURE0 + texUnit);
    glBindTexture(GL_TEXTURE_2D, texID);
}

void Texture::TexUnbind(int texUnit/* = 0 */)
{
    glActiveTexture(GL_TEXTURE0 + texUnit);
    glBindTexture(GL_TEXTURE_2D, 0);
}

Texture Texture::CreateFromColor(const glm::vec3& rgb)
{
    Texture tex;

    glGenTextures(1, &tex.m_texID);
    glBindTexture(GL_TEXTURE_2D, tex.m_texID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    tex.m_width = tex.m_height = 1;
    tex.m_channels = 3;

    uint8_t data[] =
    {
        static_cast<uint8_t>(rgb.x * 255),
        static_cast<uint8_t>(rgb.y * 255),
        static_cast<uint8_t>(rgb.z * 255)
    };

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex.m_width, tex.m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

    glBindTexture(GL_TEXTURE_2D, 0);

    return tex;
}

Texture Texture::CreateFromColor(const glm::vec4& rgba)
{
    Texture tex;

    glGenTextures(1, &tex.m_texID);
    glBindTexture(GL_TEXTURE_2D, tex.m_texID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    tex.m_width = tex.m_height = 1;
    tex.m_channels = 4;

    uint8_t data[] =
    {
        static_cast<uint8_t>(rgba.x * 255),
        static_cast<uint8_t>(rgba.y * 255),
        static_cast<uint8_t>(rgba.z * 255),
        static_cast<uint8_t>(rgba.w * 255)
    };

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex.m_width, tex.m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    glBindTexture(GL_TEXTURE_2D, 0);

    return tex;
}

