//
//  Mesh.h
//  KFRDemo
//
//  Created by klsoline on 2023/09/04.
//

#pragma once

#include <vector>
#include <string>
#include <memory>

#include <glad/glad.h>

struct Mesh
{
    GLenum drawMode;

    int attrSize;
    std::string attrDesc;

    std::shared_ptr<std::vector<float>> vertices;
    std::shared_ptr<std::vector<uint32_t>> indices;
};

