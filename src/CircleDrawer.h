//
//  CircleDrawer.h
//  KFRDemo
//
//  Created by klsoline on 2023/09/04.
//

#pragma once

#include "Circle.h"
#include "MeshDrawer.h"

class CircleDrawer : public MeshDrawer
{
public:
    bool Init(const Shader& shader) override;
    void Draw() override;

    void Setup(const Circle& circle, const glm::vec3& color);
    void Setup(float border, const glm::vec3& borderColor);

    void Resize(int width, int height);

private:
    void UpdateVao() override;
};

