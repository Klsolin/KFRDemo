//
//  CircleDrawer.cpp
//  KFRDemo
//
//  Created by klsoline on 2023/09/04.
//

#include "CircleDrawer.h"

#include <iterator>

bool CircleDrawer::Init(const Shader& shader)
{
    if(!MeshDrawer::Init(shader))
    {
        return false;
    }

    m_shader.Bind();
    m_shader.SetUniform("uBorder", 0.0f);
    m_shader.SetUniform("uAntialias", 2.0f);
    m_shader.Unbind();

    const float vertexData[] = {-1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, -1.0f};
    const uint32_t indexData[] = {0, 1, 2, 2, 1, 3};

    Mesh mesh
    {
        GL_TRIANGLES, 2, "V2",
        std::make_shared<std::vector<float>>(std::begin(vertexData), std::end(vertexData)),
        std::make_shared<std::vector<uint32_t>>(std::begin(indexData), std::end(indexData))
    };

    MeshDrawer::Update(mesh);

    return true;
}

void CircleDrawer::Draw()
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    MeshDrawer::Draw();

    glDisable(GL_BLEND);
}

void CircleDrawer::Setup(const Circle& circle, const glm::vec3& color)
{
    m_shader.Bind();
    m_shader.SetUniform("uPos", circle.x, circle.y);
    m_shader.SetUniform("uInnerRadius", circle.innerRadius);
    m_shader.SetUniform("uOuterRadius", circle.outerRadius);
    m_shader.SetUniform("uColor", color);
    m_shader.Unbind();
}

void CircleDrawer::Setup(float border, const glm::vec3& borderColor)
{
    m_shader.Bind();
    m_shader.SetUniform("uBorder", border);
    m_shader.SetUniform("uBorderColor", borderColor);
    m_shader.Unbind();
}

void CircleDrawer::Resize(int width, int height)
{
    m_shader.Bind();
    m_shader.SetUniform("uRes", float(width), float(height));
    m_shader.Unbind();
}

void CircleDrawer::UpdateVao()
{
    glBindVertexArray(m_vao);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);

    m_shader.SetAttrib("aPos", 2, GL_FLOAT, GL_FALSE, m_attrSize * sizeof(float), (const void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

