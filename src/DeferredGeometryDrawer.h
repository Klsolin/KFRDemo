//
//  DeferredGeometryDrawer.h
//  KFRDemo
//
//  Created by klsoline on 2023/09/04.
//

#pragma once

#include <string>
#include <unordered_map>

#include "Texture.h"
#include "GBuffer.h"
#include "ObjModel.h"
#include "MeshDrawer.h"

class DeferredGeometryDrawer : public MeshDrawer
{
public:
    DeferredGeometryDrawer();

    bool Init(const Shader& shader) override;
    void Destroy() override;
    void Draw() override;

    void Setup(const ObjModel& objModel);
    void Setup(float downscaleRatio);
    void Setup(const glm::mat4& modelMatrix, const glm::mat4& viewMatrix, const glm::mat4& projMatrix);

    void Resize(int width, int height);

    GBuffer Output() const;

private:
    Material m_blackMat, m_whiteMat;
    Texture m_blackTex, m_whiteTex;

    ObjModel m_objModel;
    std::unordered_map<std::string, Texture> m_texMap;

    int m_resX, m_resY;
    float m_downscaleRatio;

    GBuffer m_outGBuf;

    void UpdateVao() override;
};

