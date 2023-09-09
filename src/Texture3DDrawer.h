//
//  Texture3DDrawer.h
//  KFRDemo
//
//  Created by klsoline on 2023/09/04.
//

#pragma once

#include "Texture.h"
#include "MeshDrawer.h"

class Texture3DDrawer : public MeshDrawer
{
public:
    Texture3DDrawer();

    void Draw() override;

    void Setup(const Texture& tex);
    void Setup(GLuint texID, bool hasAlpha = false);
    void Setup(const glm::mat4& modelMatrix, const glm::mat4& viewMatrix, const glm::mat4& projMatrix);

private:
    GLuint m_texID;
    bool m_hasAlpha;

    void UpdateVao() override;
};

