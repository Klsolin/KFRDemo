//
//  KFRPass2Drawer.h
//  KFRDemo
//
//  Created by klsoline on 2023/09/04.
//

#pragma once

#include "KFRParam.h"
#include "FrameBuffer.h"
#include "MeshDrawer.h"

class KFRPass2Drawer : public MeshDrawer
{
public:
    KFRPass2Drawer();

    void Destroy() override;
    void Draw() override;

    void Setup(const FrameBuffer& fbo);
    void Setup(const KFRParam& param);
    void Setup(int gazePointX, int gazePointY);

    void Resize(int width, int height);

    FrameBuffer Output() const;

private:
    KFRParam m_param;
    FrameBuffer m_inFbo, m_outFbo;
};

