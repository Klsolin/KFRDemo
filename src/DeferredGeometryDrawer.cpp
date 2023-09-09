//
//  DeferredGeometryDrawer.cpp
//  KFRDemo
//
//  Created by klsoline on 2023/09/04.
//

#include "DeferredGeometryDrawer.h"

#include <algorithm>

DeferredGeometryDrawer::DeferredGeometryDrawer()
    : m_resX(1), m_resY(1)
    , m_downscaleRatio(1.0f)
{

}

bool DeferredGeometryDrawer::Init(const Shader& shader)
{
    if(!MeshDrawer::Init(shader))
    {
        return false;
    }

    m_shader.Bind();
    m_shader.SetUniform("sDiffMapTex", 0);
    m_shader.Unbind();

    m_blackMat.Ka = glm::vec3(0.0f);
    m_blackMat.Kd = glm::vec3(0.0f);
    m_blackMat.Ks = glm::vec3(0.0f);
    m_blackMat.shininess = 0.0f;

    m_whiteMat.Ka = glm::vec3(1000.0f);
    m_whiteMat.Kd = glm::vec3(1000.0f);
    m_whiteMat.Ks = glm::vec3(1000.0f);
    m_whiteMat.shininess = 0.0f;

    m_blackTex = Texture::CreateFromColor(glm::vec3(0.0f));
    m_whiteTex = Texture::CreateFromColor(glm::vec3(1.0f));

    return true;
}

void DeferredGeometryDrawer::Destroy()
{
    m_blackTex.Unload();
    m_whiteTex.Unload();

    m_objModel.Unload();

    for(auto& pair : m_texMap)
    {
        pair.second.Unload();
    }

    std::unordered_map<std::string, Texture>().swap(m_texMap);

    m_outGBuf.Destroy();
}

void DeferredGeometryDrawer::Draw()
{
    if(m_objModel.GetObjects().empty() || !m_outGBuf.IsValid())
    {
        return;
    }

    m_outGBuf.Bind();

    glViewport(0, 0, m_outGBuf.GetWidth(), m_outGBuf.GetHeight());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const auto& materials = m_objModel.GetMaterials();
    const auto& objects = m_objModel.GetObjects();

    for(const auto& object : objects)
    {
        for(const auto& group : object.groups)
        {
            for(const auto& batch : group.batches)
            {
                Material mat = m_blackMat;
                Texture tex = m_blackTex;

                if(batch.matId >= 0)
                {
                    const auto iter = m_texMap.find(batch.matName);
                    const bool valid = (iter != m_texMap.end() && iter->second.IsValid());

                    mat = (valid? materials[batch.matId] : m_whiteMat);
                    tex = (valid? iter->second : m_whiteTex);
                }

                m_shader.Bind();
                m_shader.SetUniform("uMat.Ka", mat.Ka);
                m_shader.SetUniform("uMat.Kd", mat.Kd);
                m_shader.SetUniform("uMat.Ks", mat.Ks);
                m_shader.SetUniform("uMat.shininess", mat.shininess);
                m_shader.Unbind();

                tex.Bind();

                for(const auto& mesh : batch.meshes)
                {
                    Update(mesh);
                    MeshDrawer::Draw();
                }

                tex.Unbind();
            }
        }
    }

    m_outGBuf.Unbind();
}

void DeferredGeometryDrawer::Setup(const ObjModel& objModel)
{
    m_objModel = objModel;

    if(m_objModel.IsVertexPool())
    {
        auto baseMesh = m_objModel.GetBaseMesh();
        baseMesh.indices = nullptr;

        Update(baseMesh);

        auto& objects = m_objModel.GetObjects();

        for(auto& object : objects)
        {
            for(auto& group : object.groups)
            {
                for(auto& batch : group.batches)
                {
                    for(auto& mesh : batch.meshes)
                    {
                        mesh.vertices = nullptr;
                    }
                }
            }
        }
    }

    for(auto& pair : m_texMap)
    {
        pair.second.Unload();
    }

    std::unordered_map<std::string, Texture>().swap(m_texMap);

    const auto& materials = m_objModel.GetMaterials();

    for(const auto& mat : materials)
    {
        const auto result = m_texMap.emplace(mat.name, Texture{});

        if(result.second)
        {
            result.first->second.Load(mat.diffMapName);
        }
    }
}

void DeferredGeometryDrawer::Setup(float downscaleRatio)
{
    m_downscaleRatio = downscaleRatio;

    m_outGBuf.Init(std::max(1, int(m_resX / m_downscaleRatio)),
                   std::max(1, int(m_resY / m_downscaleRatio)));
}

void DeferredGeometryDrawer::Setup(const glm::mat4& modelMatrix, const glm::mat4& viewMatrix, const glm::mat4& projMatrix)
{
    const glm::mat4 mvpMatrix = projMatrix * viewMatrix * modelMatrix;
    const glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(modelMatrix)));

    m_shader.Bind();
    m_shader.SetUniform("uMVPMatrix", mvpMatrix);
    m_shader.SetUniform("uModelMatrix", modelMatrix);
    m_shader.SetUniform("uNormalMatrix", normalMatrix);
    m_shader.Unbind();
}

void DeferredGeometryDrawer::Resize(int width, int height)
{
    m_resX = width;
    m_resY = height;

    m_outGBuf.Init(std::max(1, int(m_resX / m_downscaleRatio)),
                   std::max(1, int(m_resY / m_downscaleRatio)));
}

GBuffer DeferredGeometryDrawer::Output() const
{
    return m_outGBuf;
}

void DeferredGeometryDrawer::UpdateVao()
{
    glBindVertexArray(m_vao);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);

    m_shader.SetAttrib("aPos", 3, GL_FLOAT, GL_FALSE, m_attrSize * sizeof(float), (const void*)0);
    m_shader.SetAttrib("aTexCoord", 2, GL_FLOAT, GL_FALSE, m_attrSize * sizeof(float), (const void*)(3 * sizeof(float)));
    m_shader.SetAttrib("aNormal", 3, GL_FLOAT, GL_FALSE, m_attrSize * sizeof(float), (const void*)(5 * sizeof(float)));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

