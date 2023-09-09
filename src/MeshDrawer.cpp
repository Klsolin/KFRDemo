//
//  MeshDrawer.cpp
//  KFRDemo
//
//  Created by klsoline on 2023/09/04.
//

#include "MeshDrawer.h"

#include <iterator>

MeshDrawer::MeshDrawer()
    : m_drawMode(GL_TRIANGLES)
    , m_attrSize(0)
    , m_attrDesc("")
    , m_vao(0), m_vbo(0), m_ibo(0)
    , m_totalVboSize(0), m_currVboSize(0)
    , m_totalIboSize(0), m_currIboSize(0)
{

}

MeshDrawer::~MeshDrawer()
{

}

bool MeshDrawer::Init(const Shader& shader)
{
    if(!shader.IsValid())
    {
        return false;
    }

    m_shader = shader;

    m_shader.Bind();
    m_shader.SetUniform("sTex", 0);
    m_shader.Unbind();

    const float vertexData[] =
    {
        -1.0f, 1.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, -1.0f, 1.0f, 0.0f
    };

    const uint32_t indexData[] = {0, 1, 2, 2, 1, 3};

    Mesh mesh{
        GL_TRIANGLES, 4, "V2_T2",
        std::make_shared<std::vector<float>>(std::begin(vertexData), std::end(vertexData)),
        std::make_shared<std::vector<uint32_t>>(std::begin(indexData), std::end(indexData))
    };

    Update(mesh);

    return true;
}

void MeshDrawer::Destroy()
{
    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers(1, &m_vbo);
    glDeleteBuffers(1, &m_ibo);

    m_vao = m_vbo = m_ibo = 0;
    m_totalVboSize = m_currVboSize = 0;
    m_totalIboSize = m_currIboSize = 0;
}

void MeshDrawer::Draw()
{
    if(!IsValid())
    {
        return;
    }

    m_shader.Bind();

    glBindVertexArray(m_vao);
    glDrawElements(m_drawMode, m_currIboSize / sizeof(uint32_t), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);

    m_shader.Unbind();
}

void MeshDrawer::Update(const Mesh& mesh, bool exactFit/* = false */)
{
    bool needUpdateVao = (m_attrDesc != mesh.attrDesc);

    m_drawMode = mesh.drawMode;
    m_attrSize = mesh.attrSize;
    m_attrDesc = mesh.attrDesc;

    if(glIsVertexArray(m_vao) != GL_TRUE)
    {
        glGenVertexArrays(1, &m_vao);

        // the vao may not actually be fully initialized until it's bound for the first time
        glBindVertexArray(m_vao);
        glBindVertexArray(0);
    }

    if(mesh.vertices != nullptr)
    {
        m_currVboSize = (*mesh.vertices).size() * sizeof(float);

        if(exactFit? m_currVboSize != m_totalVboSize : m_currVboSize > m_totalVboSize)
        {
            glDeleteBuffers(1, &m_vbo);
            glGenBuffers(1, &m_vbo);
            glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
            glBufferData(GL_ARRAY_BUFFER, m_currVboSize, (*mesh.vertices).data(), GL_STATIC_DRAW);

            m_totalVboSize = m_currVboSize;
            needUpdateVao = true;
        }
        else
        {
            glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
            glBufferSubData(GL_ARRAY_BUFFER, 0, m_currVboSize, (*mesh.vertices).data());
        }

        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    if(mesh.indices != nullptr)
    {
        m_currIboSize = (*mesh.indices).size() * sizeof(uint32_t);

        if(exactFit? m_currIboSize != m_totalIboSize : m_currIboSize > m_totalIboSize)
        {
            glDeleteBuffers(1, &m_ibo);
            glGenBuffers(1, &m_ibo);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_currIboSize, (*mesh.indices).data(), GL_STATIC_DRAW);

            m_totalIboSize = m_currIboSize;
            needUpdateVao = true;
        }
        else
        {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
            glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, m_currIboSize, (*mesh.indices).data());
        }

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    if(needUpdateVao && IsValid())
    {
        UpdateVao();
    }
}

bool MeshDrawer::IsValid() const
{
    return m_shader.IsValid() &&
           glIsVertexArray(m_vao) == GL_TRUE &&
           glIsBuffer(m_vbo) == GL_TRUE &&
           glIsBuffer(m_ibo) == GL_TRUE;
}

Shader MeshDrawer::GetShader() const
{
    return m_shader;
}

void MeshDrawer::UpdateVao()
{
    glBindVertexArray(m_vao);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);

    m_shader.SetAttrib("aPos", 2, GL_FLOAT, GL_FALSE, m_attrSize * sizeof(float), (const void*)0);
    m_shader.SetAttrib("aTexCoord", 2, GL_FLOAT, GL_FALSE, m_attrSize * sizeof(float), (const void*)(2 * sizeof(float)));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

