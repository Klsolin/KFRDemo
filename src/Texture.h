//
//  Texture.h
//  KFRDemo
//
//  Created by klsoline on 2023/09/04.
//

#pragma once

#include <string>

#include <glad/glad.h>
#include <glm/glm.hpp>

class Texture
{
public:
    Texture();

    bool Load(const std::string& texPath, bool genMipmaps = false);
    void Unload();

    GLuint GetTexID() const;

    int GetWidth() const;
    int GetHeight() const;
    int GetChannels() const;

    bool IsValid() const;

    void Bind(int texUnit = 0);
    void Unbind(int texUnit = 0);

    static bool IsTexValid(GLuint texID);

    static void TexBind(GLuint texID, int texUnit = 0);
    static void TexUnbind(int texUnit = 0);

    static Texture CreateFromColor(const glm::vec3& rgb);
    static Texture CreateFromColor(const glm::vec4& rgba);

private:
    GLuint m_texID;
    int m_width, m_height, m_channels;
};

