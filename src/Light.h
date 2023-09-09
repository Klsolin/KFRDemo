//
//  Light.h
//  KFRDemo
//
//  Created by klsoline on 2023/09/04.
//

#pragma once

#include <glm/glm.hpp>

struct Light
{
    int type;
    glm::vec3 pos;
    glm::vec3 dir;
    glm::vec3 La;
    glm::vec3 Ld;
    glm::vec3 Ls;
    glm::vec3 att;
    float innerAngle;
    float outerAngle;
};

