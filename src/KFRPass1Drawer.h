//
//  KFRPass1Drawer.h
//  KFRDemo
//
//  Created by klsoline on 2023/09/04.
//

#pragma once

#include "GBuffer.h"
#include "KFRParam.h"
#include "MeshDrawer.h"

class KFRPass1Drawer : public MeshDrawer
{
public:
    KFRPass1Drawer();

    bool Init(const Shader& shader) override;
    void Destroy() override;
    void Draw() override;

    void Setup(const GBuffer& gbuf);
    void Setup(const KFRParam& param);
    void Setup(int gazePointX, int gazePointY);

    void Resize(int width, int height);

    GBuffer Output() const;

private:
    KFRParam m_param;
    GBuffer m_inGBuf, m_outGBuf;
};

