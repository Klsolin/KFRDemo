//
//  ForwardDrawer.h
//  KFRDemo
//
//  Created by klsoline on 2023/09/04.
//

#pragma once

#include <vector>
#include <string>
#include <unordered_map>

#include "Light.h"
#include "Texture.h"
#include "ObjModel.h"
#include "MeshDrawer.h"

class ForwardDrawer : public MeshDrawer
{
public:
    bool Init(const Shader& shader) override;
    void Destroy() override;
    void Draw() override;

    void Setup(const ObjModel& objModel);
    void Setup(const glm::vec3& eyePos);
    void Setup(const std::vector<Light>& lights, int lightCount);
    void Setup(const glm::mat4& modelMatrix, const glm::mat4& viewMatrix, const glm::mat4& projMatrix);

private:
    Material m_blackMat, m_whiteMat;
    Texture m_blackTex, m_whiteTex;

    ObjModel m_objModel;
    std::unordered_map<std::string, Texture> m_texMap;

    void UpdateVao() override;
};

