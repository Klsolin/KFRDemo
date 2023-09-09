//
//  Material.h
//  KFRDemo
//
//  Created by klsoline on 2023/09/04.
//

#pragma once

#include <string>

#include <glm/glm.hpp>

struct Material
{
    std::string name;

    glm::vec3 Ka;
    glm::vec3 Kd;
    glm::vec3 Ks;
    float shininess;

    std::string ambiMapName;
    std::string diffMapName;
    std::string specMapName;
    std::string normalMapName;
};

