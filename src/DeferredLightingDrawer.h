//
//  DeferredLightingDrawer.h
//  KFRDemo
//
//  Created by klsoline on 2023/09/04.
//

#pragma once

#include <vector>

#include "Light.h"
#include "GBuffer.h"
#include "MeshDrawer.h"
#include "FrameBuffer.h"

class DeferredLightingDrawer : public MeshDrawer
{
public:
    bool Init(const Shader& shader) override;
    void Destroy() override;
    void Draw() override;

    void Setup(const GBuffer& gbuf);
    void Setup(const glm::vec3& eyePos);
    void Setup(const std::vector<Light>& lights, int lightCount);

    FrameBuffer Output() const;

private:
    GBuffer m_inGBuf;
    FrameBuffer m_outFbo;
};

