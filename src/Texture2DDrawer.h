//
//  Texture2DDrawer.h
//  KFRDemo
//
//  Created by klsoline on 2023/09/04.
//

#pragma once

#include "Texture.h"
#include "MeshDrawer.h"

class Texture2DDrawer : public MeshDrawer
{
public:
    Texture2DDrawer();

    void Draw() override;
    void DrawFullScreen();

    void Setup(const Texture& tex);
    void Setup(GLuint texID, bool hasAlpha = false);
    void Setup(float x, float y, float w, float h);

    void Resize(int width, int height);

private:
    GLuint m_texID;
    bool m_hasAlpha;

    int m_resX, m_resY;
};

