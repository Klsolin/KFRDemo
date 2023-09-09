//
//  KFREffect.cpp
//  KFRDemo
//
//  Created by klsoline on 2023/09/04.
//

#include "KFREffect.h"

#include <cmath>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#define TWO_PI 6.28318530718

#define MAX_LIGHT_COUNT 128
#define DEFAULT_LIGHT_COUNT 64

KFREffect::KFREffect()
    : m_fps(0.0f)
    , m_resX(1), m_resY(1)
    , m_cursorX(-1), m_cursorY(-1)
    , m_keyPressed{}
    , m_eyePos{-2.0f, 1.0f, 1.0f}
    , m_uiEnabled(true)
    , m_wireframeEnabled(false)
    , m_deferredShadingEnabled(true)
    , m_kfrEnabled(true)
    , m_kfrDownscaleEnabled(false)
    , m_kfrParam{4.0f, 2.0f}
    , m_lightCount(DEFAULT_LIGHT_COUNT)
    , m_gazePointX(1446)
    , m_gazePointY(568)
{

}

bool KFREffect::Init(int width, int height)
{
    glDepthFunc(GL_LEQUAL);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    if(!m_objModel.Load("../../res/models/sponza-pbr.obj"))
    {
        return false;
    }

    if(!m_circleShader.Load("../../res/shaders/Circle.vert", "../../res/shaders/Circle.frag") ||
       !m_tex2DShader.Load("../../res/shaders/Texture2D.vert", "../../res/shaders/Texture2D.frag") ||
       !m_forwardShader.Load("../../res/shaders/Forward.vert", "../../res/shaders/Forward.frag") ||
       !m_deferredGeometryShader.Load("../../res/shaders/DeferredGeometry.vert", "../../res/shaders/DeferredGeometry.frag") ||
       !m_deferredLightingShader.Load("../../res/shaders/DeferredLighting.vert", "../../res/shaders/DeferredLighting.frag") ||
       !m_kfrPass1Shader.Load("../../res/shaders/KFRPass1.vert", "../../res/shaders/KFRPass1.frag") ||
       !m_kfrPass2Shader.Load("../../res/shaders/KFRPass2.vert", "../../res/shaders/KFRPass2.frag"))
    {
        return false;
    }

    if(!m_circleDrawer.Init(m_circleShader) ||
       !m_tex2DDrawer.Init(m_tex2DShader) ||
       !m_forwardDrawer.Init(m_forwardShader) ||
       !m_deferredGeometryDrawer.Init(m_deferredGeometryShader) ||
       !m_deferredLightingDrawer.Init(m_deferredLightingShader) ||
       !m_kfrPass1Drawer.Init(m_kfrPass1Shader) ||
       !m_kfrPass2Drawer.Init(m_kfrPass2Shader))
    {
        return false;
    }

    m_viewMat = glm::lookAt(m_eyePos,
                            glm::vec3(0.0f, 1.0f, 0.0f),
                            glm::vec3(0.0f, 1.0f, 0.0f));

    m_lights.resize(MAX_LIGHT_COUNT);
    UpdateLights();

    m_forwardDrawer.Setup(m_objModel);
    m_forwardDrawer.Setup(m_eyePos);

    const float downscaleRatio = ((m_kfrEnabled && m_kfrDownscaleEnabled)? m_kfrParam.downscaleRatio : 1.0f);

    m_deferredGeometryDrawer.Setup(m_objModel);
    m_deferredGeometryDrawer.Setup(downscaleRatio);

    m_deferredLightingDrawer.Setup(m_eyePos);

    m_kfrPass1Drawer.Setup(m_kfrParam);
    m_kfrPass2Drawer.Setup(m_kfrParam);

    Resize(width, height);

    return true;
}

void KFREffect::Destroy()
{
    m_objModel.Unload();

    m_circleShader.Unload();
    m_tex2DShader.Unload();
    m_forwardShader.Unload();
    m_deferredGeometryShader.Unload();
    m_deferredLightingShader.Unload();
    m_kfrPass1Shader.Unload();
    m_kfrPass2Shader.Unload();

    m_circleDrawer.Destroy();
    m_tex2DDrawer.Destroy();
    m_forwardDrawer.Destroy();
    m_deferredGeometryDrawer.Destroy();
    m_deferredLightingDrawer.Destroy();
    m_kfrPass1Drawer.Destroy();
    m_kfrPass2Drawer.Destroy();
}

void KFREffect::Draw()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);

    if(m_wireframeEnabled)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }

    if(m_deferredShadingEnabled)
    {
        GLuint outTexID = 0;

        const float downscaleRatio = ((m_kfrEnabled && m_kfrDownscaleEnabled)? m_kfrParam.downscaleRatio : 1.0f);

        m_deferredGeometryDrawer.Setup(downscaleRatio);
        m_deferredGeometryDrawer.Setup(glm::mat4(1.0f), m_viewMat, m_projMat);
        m_deferredGeometryDrawer.Draw();

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        glDisable(GL_DEPTH_TEST);

        if(m_kfrEnabled)
        {
            m_kfrPass1Drawer.Setup(m_deferredGeometryDrawer.Output());
            m_kfrPass1Drawer.Setup(m_kfrParam);
            m_kfrPass1Drawer.Setup(m_gazePointX, m_gazePointY);
            m_kfrPass1Drawer.Draw();

            m_deferredLightingDrawer.Setup(m_kfrPass1Drawer.Output());
            m_deferredLightingDrawer.Draw();

            m_kfrPass2Drawer.Setup(m_deferredLightingDrawer.Output());
            m_kfrPass2Drawer.Setup(m_kfrParam);
            m_kfrPass2Drawer.Setup(m_gazePointX, m_gazePointY);
            m_kfrPass2Drawer.Draw();

            outTexID = m_kfrPass2Drawer.Output().GetTexID();
        }
        else
        {
            m_deferredLightingDrawer.Setup(m_deferredGeometryDrawer.Output());
            m_deferredLightingDrawer.Draw();

            outTexID = m_deferredLightingDrawer.Output().GetTexID();
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, m_resX, m_resY);

        m_tex2DDrawer.Setup(outTexID);
        m_tex2DDrawer.DrawFullScreen();

        if(m_kfrEnabled)
        {
            m_circleDrawer.Setup({float(m_gazePointX), float(m_gazePointY), 4.0f, 12.0f}, glm::vec3(0.0f, 1.0f, 0.0f));
            m_circleDrawer.Setup(2.0f, glm::vec3(0.0f, 0.0f, 1.0f));
            m_circleDrawer.Draw();
        }
    }
    else
    {
        m_forwardDrawer.Setup(glm::mat4(1.0f), m_viewMat, m_projMat);
        m_forwardDrawer.Draw();

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    DrawUI();
    DrawCursor();
}

void KFREffect::Update(float dt)
{
    UpdateFPS(dt);

    if(m_keyPressed[GLFW_KEY_LEFT])
    {
        if(m_lightCount > 1)
        {
            --m_lightCount;
            UpdateLights();
        }
    }
    else if(m_keyPressed[GLFW_KEY_RIGHT])
    {
        if(m_lightCount < MAX_LIGHT_COUNT)
        {
            ++m_lightCount;
            UpdateLights();
        }
    }
}

void KFREffect::Resize(int width, int height)
{
    m_resX = width;
    m_resY = height;

    glViewport(0, 0, m_resX, m_resY);

    m_projMat = glm::perspective(glm::radians(45.0f),
                                 float(m_resX) / float(m_resY),
                                 0.001f, 1000.0f);

    m_circleDrawer.Resize(m_resX, m_resY);
    m_tex2DDrawer.Resize(m_resX, m_resY);
    m_deferredGeometryDrawer.Resize(m_resX, m_resY);
    m_kfrPass1Drawer.Resize(m_resX, m_resY);
    m_kfrPass2Drawer.Resize(m_resX, m_resY);
}

void KFREffect::CursorMove(int x, int y)
{
    m_cursorX = x;
    m_cursorY = y;
}

void KFREffect::CursorDrag(int x, int y)
{
    m_cursorX = x;
    m_cursorY = y;
}

void KFREffect::CursorClick(int x, int y, int button)
{
    m_cursorX = x;
    m_cursorY = y;

    if(button == GLFW_MOUSE_BUTTON_RIGHT)
    {
        m_gazePointX = m_cursorX;
        m_gazePointY = m_cursorY;
    }
}

void KFREffect::KeyPress(int key)
{
    m_keyPressed[key] = true;

    if(key == GLFW_KEY_R)
    {
        Reset();
    }
    else if(key == GLFW_KEY_U)
    {
        m_uiEnabled = !m_uiEnabled;
    }
    else if(key == GLFW_KEY_W)
    {
        m_wireframeEnabled = !m_wireframeEnabled;
    }
    else if(key == GLFW_KEY_D)
    {
        m_deferredShadingEnabled = !m_deferredShadingEnabled;
        UpdateLights();
    }
    else if(key == GLFW_KEY_K)
    {
        m_kfrEnabled = !m_kfrEnabled;
    }
    else if(key == GLFW_KEY_S)
    {
        m_kfrDownscaleEnabled = !m_kfrDownscaleEnabled;
    }
}

void KFREffect::KeyRelease(int key)
{
    m_keyPressed[key] = false;
}

void KFREffect::Reset()
{
    m_uiEnabled = true;

    m_wireframeEnabled = false;
    m_deferredShadingEnabled = true;

    m_kfrEnabled = true;
    m_kfrDownscaleEnabled = false;

    m_kfrParam.kernelParam = 4.0f;
    m_kfrParam.downscaleRatio = 2.0f;

    m_lightCount = DEFAULT_LIGHT_COUNT;
    UpdateLights();

    m_gazePointX = 1446;
    m_gazePointY = 568;
}

void KFREffect::DrawUI()
{
    if(!m_uiEnabled)
    {
        return;
    }

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::SetNextWindowPos(ImVec2(8, 8), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(256, 282), ImGuiCond_Once);
    ImGui::SetNextWindowBgAlpha(0.75f);

    if(ImGui::Begin("UI", nullptr, ImGuiWindowFlags_NoCollapse))
    {
        const float regionWidth = ImGui::GetWindowContentRegionWidth();

        // ImGui::Text("FPS: %.2f", ImGui::GetIO().Framerate);
        ImGui::Text("FPS: %.2f", m_fps);

        ImGui::SetNextItemOpen(true, ImGuiCond_Once);

        if(ImGui::CollapsingHeader("Config"))
        {
            ImGui::Separator();

            ImGui::Text("Wireframe Enabled: ");
            ImGui::SameLine();
            ImGui::Checkbox("##Wireframe Enabled", &m_wireframeEnabled);

            ImGui::Text("Deferred Shading Enabled: ");
            ImGui::SameLine();
            
            if(ImGui::Checkbox("##Deferred Shading Enabled", &m_deferredShadingEnabled))
            {
                UpdateLights();
            }

            ImGui::Separator();

            ImGui::Text("KFR Enabled: ");
            ImGui::SameLine();
            ImGui::Checkbox("##KFR Enabled", &m_kfrEnabled);

            ImGui::Text("KFR Downscale Enabled: ");
            ImGui::SameLine();
            ImGui::Checkbox("##KFR Downscale Enabled", &m_kfrDownscaleEnabled);

            ImGui::PushItemWidth(regionWidth * 0.6f);

            ImGui::Text("Alpha: ");
            ImGui::SameLine();
            ImGui::SliderFloat("##Alpha", &m_kfrParam.kernelParam, 1.0f, 5.0f);

            ImGui::Text("Sigma: ");
            ImGui::SameLine();
            ImGui::SliderFloat("##Sigma", &m_kfrParam.downscaleRatio, 1.0f, 5.0f);

            ImGui::PopItemWidth();

            ImGui::Separator();

            ImGui::PushItemWidth(regionWidth * 0.6f);

            ImGui::Text("Light Count: ");
            ImGui::SameLine();
            ImGui::SliderInt("##Light Count", &m_lightCount, 1, MAX_LIGHT_COUNT);

            ImGui::PopItemWidth();

            if(ImGui::IsItemDeactivatedAfterEdit())
            {
                UpdateLights();
            }

            ImGui::Separator();

            const std::string resetString = "Reset";
            const ImVec2 textSize = ImGui::CalcTextSize(resetString.c_str());
            const float buttonWidth = textSize.x + ImGui::GetStyle().FramePadding.x * 2.0f;

            ImGui::SetCursorPosX((ImGui::GetWindowWidth() - buttonWidth) * 0.5f);

            if(ImGui::Button(resetString.c_str(), ImVec2(buttonWidth, 0.0f)))
            {
                Reset();
            }

            ImGui::Separator();
        }
    }
    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void KFREffect::DrawCursor()
{
    if(ImGui::GetIO().WantCaptureMouse)
    {
        return;
    }

    if((m_cursorX < 0 || m_cursorX >= m_resX) ||
       (m_cursorY < 0 || m_cursorY >= m_resY))
    {
        return;
    }

    m_circleDrawer.Setup({float(m_cursorX), float(m_cursorY), 12.0f, 20.0f}, glm::vec3(1.0f, 0.0f, 0.0f));
    m_circleDrawer.Setup(2.0f, glm::vec3(0.0f, 0.0f, 1.0f));
    m_circleDrawer.Draw();
}

void KFREffect::UpdateFPS(float dt)
{
    static int totalFrameCount = 0;
    static float totalFrameTime = 0.0f;

    ++totalFrameCount;
    totalFrameTime += dt;

    if(totalFrameTime >= 1.0f)
    {
        m_fps = totalFrameCount / totalFrameTime;

        totalFrameCount = 0;
        totalFrameTime = 0.0f;
    }
}

void KFREffect::UpdateLights()
{
    const float radius = 5.0f;
    const float theta = TWO_PI / m_lightCount;

    for(int i = 0; i < m_lightCount; ++i)
    {
        m_lights[i].pos = glm::vec3(radius * std::cos(i * theta), 2.0f, radius * std::sin(i * theta));
        // m_lights[i].dir = glm::vec3(std::cos(i * theta), 0.0f, std::sin(i * theta));
        m_lights[i].La = glm::vec3(0.012f);
        m_lights[i].Ld = glm::vec3(0.012f);
        m_lights[i].Ls = glm::vec3(0.012f);
        m_lights[i].att = glm::vec3(1.0f, 0.0f, 0.0f);
    }

    if(m_deferredShadingEnabled)
    {
        m_deferredLightingDrawer.Setup(m_lights, m_lightCount);
    }
    else
    {
        m_forwardDrawer.Setup(m_lights, m_lightCount);
    }
}

