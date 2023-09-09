//
//  KFREffect.h
//  KFRDemo
//
//  Created by klsoline on 2023/09/04.
//

#pragma once

#include <array>
#include <vector>

#include "Light.h"
#include "ObjModel.h"
#include "KFRParam.h"

#include "CircleDrawer.h"
#include "Texture2DDrawer.h"
#include "ForwardDrawer.h"
#include "DeferredGeometryDrawer.h"
#include "DeferredLightingDrawer.h"
#include "KFRPass1Drawer.h"
#include "KFRPass2Drawer.h"

class KFREffect
{
public:
    KFREffect();

    bool Init(int width, int height);
    void Destroy();
    void Draw();
    void Update(float dt);

    void Resize(int width, int height);

    void CursorMove(int x, int y);
    void CursorDrag(int x, int y);
    void CursorClick(int x, int y, int button);

    void KeyPress(int key);
    void KeyRelease(int key);

private:
    float m_fps;

    int m_resX, m_resY;
    int m_cursorX, m_cursorY;
    std::array<bool, 512> m_keyPressed;

    glm::vec3 m_eyePos;
    glm::mat4 m_viewMat, m_projMat;

    bool m_uiEnabled;

    bool m_wireframeEnabled;
    bool m_deferredShadingEnabled;

    bool m_kfrEnabled;
    bool m_kfrDownscaleEnabled;

    KFRParam m_kfrParam;

    int m_lightCount;
    std::vector<Light> m_lights;

    int m_gazePointX, m_gazePointY;

    ObjModel m_objModel;

    Shader m_circleShader;
    Shader m_tex2DShader;
    Shader m_forwardShader;
    Shader m_deferredGeometryShader;
    Shader m_deferredLightingShader;
    Shader m_kfrPass1Shader;
    Shader m_kfrPass2Shader;

    CircleDrawer m_circleDrawer;
    Texture2DDrawer m_tex2DDrawer;
    ForwardDrawer m_forwardDrawer;
    DeferredGeometryDrawer m_deferredGeometryDrawer;
    DeferredLightingDrawer m_deferredLightingDrawer;
    KFRPass1Drawer m_kfrPass1Drawer;
    KFRPass2Drawer m_kfrPass2Drawer;

    void Reset();

    void DrawUI();
    void DrawCursor();

    void UpdateFPS(float dt);
    void UpdateLights();
};

