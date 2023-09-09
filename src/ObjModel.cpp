//
//  ObjModel.cpp
//  KFRDemo
//
//  Created by klsoline on 2023/09/04.
//

#include "ObjModel.h"

#include <cassert>

#include <fstream>
#include <iostream>
#include <algorithm>
#include <unordered_map>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace
{
    struct UniqIndex
    {
        uint32_t currIndex;
        std::unordered_map<std::string, uint32_t> indexMap;
    };

    std::string MtlPath(const std::string& modelPath)
    {
        return modelPath.substr(0, modelPath.find_last_of('.')) + ".mtl";
    }

    std::string MtlBaseDir(const std::string& modelPath)
    {
        const std::size_t lastSlash = modelPath.find_last_of("\\/");
        const bool slashFound = (lastSlash != std::string::npos);

        return (slashFound? modelPath.substr(0, lastSlash) : ".") + "/";
    }

    std::string MtlBaseName(const std::string& modelPath)
    {
        const std::size_t lastSlash = modelPath.find_last_of("\\/");
        const bool slashFound = (lastSlash != std::string::npos);

        const std::string modelBaseName = (slashFound? modelPath.substr(lastSlash + 1) : modelPath);

        return modelBaseName.substr(0, modelBaseName.find_last_of('.')) + ".mtl";
    }

    template<typename ElementType>
    ElementType& FetchElementByKey(std::vector<ElementType>& elements, const typename ElementType::KeyType& key)
    {
        auto iter = std::find_if(elements.rbegin(), elements.rend(),
                                 [key](const ElementType& element) { return element.Key() == key; });

        if(iter == elements.rend())
        {
            elements.emplace_back(ElementType::Create(key));
            iter = elements.rbegin();
        }

        return *iter;
    }

    void FillMaterials(const std::string& mtlBaseDir,
                       const std::vector<tinyobj::material_t>& srcMaterials,
                       std::vector<Material>& dstMaterials)
    {
        const int materialCount = int(srcMaterials.size());

        dstMaterials.resize(materialCount);

        for(int m = 0; m < materialCount; ++m)
        {
            dstMaterials[m].name = srcMaterials[m].name;

            std::copy_n(srcMaterials[m].ambient, 3, glm::value_ptr(dstMaterials[m].Ka));
            std::copy_n(srcMaterials[m].diffuse, 3, glm::value_ptr(dstMaterials[m].Kd));
            std::copy_n(srcMaterials[m].specular, 3, glm::value_ptr(dstMaterials[m].Ks));
            dstMaterials[m].shininess = srcMaterials[m].shininess;

            dstMaterials[m].ambiMapName = mtlBaseDir + srcMaterials[m].ambient_texname;
            dstMaterials[m].diffMapName = mtlBaseDir + srcMaterials[m].diffuse_texname;
            dstMaterials[m].specMapName = mtlBaseDir + srcMaterials[m].specular_texname;
            dstMaterials[m].normalMapName = mtlBaseDir + srcMaterials[m].normal_texname;
        }
    }

    void ComputeNormals(tinyobj::attrib_t& attrib, std::vector<tinyobj::shape_t>& shapes)
    {
        attrib.normals.resize(attrib.vertices.size(), 0.0f);

        for(auto& shape : shapes)
        {
            const int indexCount = int(shape.mesh.indices.size());

            for(int i = 0; i < indexCount; i += 3)
            {
                auto& index0 = shape.mesh.indices[i + 0];
                auto& index1 = shape.mesh.indices[i + 1];
                auto& index2 = shape.mesh.indices[i + 2];

                const int v0 = index0.vertex_index;
                const int v1 = index1.vertex_index;
                const int v2 = index2.vertex_index;

                index0.normal_index = v0;
                index1.normal_index = v1;
                index2.normal_index = v2;

                const glm::vec3 vertex0(attrib.vertices[v0 * 3 + 0],
                                        attrib.vertices[v0 * 3 + 1],
                                        attrib.vertices[v0 * 3 + 2]);

                const glm::vec3 vertex1(attrib.vertices[v1 * 3 + 0],
                                        attrib.vertices[v1 * 3 + 1],
                                        attrib.vertices[v1 * 3 + 2]);

                const glm::vec3 vertex2(attrib.vertices[v2 * 3 + 0],
                                        attrib.vertices[v2 * 3 + 1],
                                        attrib.vertices[v2 * 3 + 2]);

                const glm::vec3 normal = glm::normalize(glm::cross(glm::normalize(vertex1 - vertex0),
                                                                   glm::normalize(vertex2 - vertex0)));

                attrib.normals[v0 * 3 + 0] += normal.x;
                attrib.normals[v0 * 3 + 1] += normal.y;
                attrib.normals[v0 * 3 + 2] += normal.z;

                attrib.normals[v1 * 3 + 0] += normal.x;
                attrib.normals[v1 * 3 + 1] += normal.y;
                attrib.normals[v1 * 3 + 2] += normal.z;

                attrib.normals[v2 * 3 + 0] += normal.x;
                attrib.normals[v2 * 3 + 1] += normal.y;
                attrib.normals[v2 * 3 + 2] += normal.z;
            }
        }

        const int vertexCount = int(attrib.vertices.size()) / 3;

        for(int v = 0; v < vertexCount; ++v)
        {
            glm::vec3 normal(attrib.normals[v * 3 + 0],
                             attrib.normals[v * 3 + 1],
                             attrib.normals[v * 3 + 2]);

            normal = glm::normalize(normal);

            attrib.normals[v * 3 + 0] = normal.x;
            attrib.normals[v * 3 + 1] = normal.y;
            attrib.normals[v * 3 + 2] = normal.z;
        }
    }

    void ComputeNormals(Mesh& mesh)
    {
        const int indexCount = int((*mesh.indices).size());

        for(int i = 0; i < indexCount; i += 3)
        {
            const auto v0 = (*mesh.indices)[i + 0];
            const auto v1 = (*mesh.indices)[i + 1];
            const auto v2 = (*mesh.indices)[i + 2];

            const glm::vec3 vertex0((*mesh.vertices)[v0 * mesh.attrSize + 0],
                                    (*mesh.vertices)[v0 * mesh.attrSize + 1],
                                    (*mesh.vertices)[v0 * mesh.attrSize + 2]);

            const glm::vec3 vertex1((*mesh.vertices)[v1 * mesh.attrSize + 0],
                                    (*mesh.vertices)[v1 * mesh.attrSize + 1],
                                    (*mesh.vertices)[v1 * mesh.attrSize + 2]);

            const glm::vec3 vertex2((*mesh.vertices)[v2 * mesh.attrSize + 0],
                                    (*mesh.vertices)[v2 * mesh.attrSize + 1],
                                    (*mesh.vertices)[v2 * mesh.attrSize + 2]);

            const glm::vec3 normal = glm::normalize(glm::cross(glm::normalize(vertex1 - vertex0),
                                                               glm::normalize(vertex2 - vertex0)));

            (*mesh.vertices)[v0 * mesh.attrSize + 5] += normal.x;
            (*mesh.vertices)[v0 * mesh.attrSize + 6] += normal.y;
            (*mesh.vertices)[v0 * mesh.attrSize + 7] += normal.z;

            (*mesh.vertices)[v1 * mesh.attrSize + 5] += normal.x;
            (*mesh.vertices)[v1 * mesh.attrSize + 6] += normal.y;
            (*mesh.vertices)[v1 * mesh.attrSize + 7] += normal.z;

            (*mesh.vertices)[v2 * mesh.attrSize + 5] += normal.x;
            (*mesh.vertices)[v2 * mesh.attrSize + 6] += normal.y;
            (*mesh.vertices)[v2 * mesh.attrSize + 7] += normal.z;
        }

        const int vertexCount = int((*mesh.vertices).size()) / mesh.attrSize;

        for(int v = 0; v < vertexCount; ++v)
        {
            glm::vec3 normal((*mesh.vertices)[v * mesh.attrSize + 5],
                             (*mesh.vertices)[v * mesh.attrSize + 6],
                             (*mesh.vertices)[v * mesh.attrSize + 7]);

            normal = glm::normalize(normal);

            (*mesh.vertices)[v * mesh.attrSize + 5] = normal.x;
            (*mesh.vertices)[v * mesh.attrSize + 6] = normal.y;
            (*mesh.vertices)[v * mesh.attrSize + 7] = normal.z;
        }
    }

    void FillPoolMesh(const tinyobj::attrib_t& attrib, const std::vector<tinyobj::shape_t>& shapes, Mesh& poolMesh)
    {
        int totalIndexCount = 0;

        for(const auto& shape : shapes)
        {
            totalIndexCount += int(shape.mesh.indices.size());
        }

        poolMesh = Mesh
        {
            GL_TRIANGLES, 9, "V3_T2_N3_FLAG1",
            std::make_shared<std::vector<float>>(),
            std::make_shared<std::vector<uint32_t>>()
        };

        (*poolMesh.vertices).reserve(totalIndexCount * poolMesh.attrSize);
        (*poolMesh.indices).reserve(totalIndexCount);

        UniqIndex uniqIndex{0, {}};

        for(const auto& shape : shapes)
        {
            for(const auto& index : shape.mesh.indices)
            {
                const int v = index.vertex_index;
                const int t = index.texcoord_index;
                const int n = index.normal_index;

                const bool hasUV = (t >= 0);
                const bool hasNormal = (n >= 0);

                const std::string indexKey = std::to_string(v) + "_" + std::to_string(t) + "_" + std::to_string(n);
                const auto result = uniqIndex.indexMap.emplace(indexKey, uniqIndex.currIndex);

                if(result.second)
                {
                    ++uniqIndex.currIndex;

                    // x y z u v nx ny nz flag
                    (*poolMesh.vertices).push_back(attrib.vertices[v * 3 + 0]);
                    (*poolMesh.vertices).push_back(attrib.vertices[v * 3 + 1]);
                    (*poolMesh.vertices).push_back(attrib.vertices[v * 3 + 2]);
                    (*poolMesh.vertices).push_back(hasUV? attrib.texcoords[t * 2 + 0] : 0.0f);
                    (*poolMesh.vertices).push_back(hasUV? attrib.texcoords[t * 2 + 1] : 0.0f);
                    (*poolMesh.vertices).push_back(hasNormal? attrib.normals[n * 3 + 0] : 0.0f);
                    (*poolMesh.vertices).push_back(hasNormal? attrib.normals[n * 3 + 1] : 0.0f);
                    (*poolMesh.vertices).push_back(hasNormal? attrib.normals[n * 3 + 2] : 1.0f);
                    (*poolMesh.vertices).push_back((hasUV? 1.0f : 0.0f) + (hasNormal? 2.0f : 0.0f));
                }

                (*poolMesh.indices).push_back(result.first->second);
            }
        }

        (*poolMesh.vertices).shrink_to_fit();
    }

    void FillObjects(
            const std::vector<tinyobj::shape_t>& shapes,
            const Mesh& poolMesh,
            ObjModel::LoadMode loadMode,
            std::vector<ObjModel::Object>& objects)
    {
        std::vector<ObjModel::Object>().swap(objects);

        int currIndex = 0;

        for(const auto& shape : shapes)
        {
            auto& object = FetchElementByKey<ObjModel::Object>(objects, shape.object_name);
            auto& group = FetchElementByKey<ObjModel::Group>(object.groups, shape.name);

            std::unordered_map<std::string, UniqIndex> matIndexMap;

            const int faceCount = int(shape.mesh.material_ids.size());
            const int indexCount = int(shape.mesh.indices.size());
            assert(faceCount * 3 == indexCount);

            for(int f = 0; f < faceCount; ++f)
            {
                const std::string& matName = shape.mesh.material_names[f];

                auto& batch = FetchElementByKey<ObjModel::Batch>(group.batches, matName);
                batch.matId = shape.mesh.material_ids[f];

                const auto matNameResult = matIndexMap.emplace(matName, UniqIndex{0, {}});

                if(matNameResult.second)
                {
                    batch.meshes.emplace_back(Mesh{GL_TRIANGLES, 9, "V3_T2_N3_FLAG1",
                                                   std::make_shared<std::vector<float>>(),
                                                   std::make_shared<std::vector<uint32_t>>()});

                    if(loadMode == ObjModel::LoadMode::VERTEX_POOL)
                    {
                        batch.meshes.back().vertices = poolMesh.vertices;
                    }
                    else if(loadMode == ObjModel::LoadMode::VERTEX_COPY)
                    {
                        (*batch.meshes.back().vertices).reserve(indexCount * batch.meshes.back().attrSize);
                    }

                    (*batch.meshes.back().indices).reserve(indexCount);
                }

                auto& mesh = batch.meshes.back();

                for(int i = 0; i < 3; ++i)
                {
                    const auto v = (*poolMesh.indices)[currIndex + i];

                    if(loadMode == ObjModel::LoadMode::VERTEX_POOL)
                    {
                        (*mesh.indices).push_back(v);
                    }
                    else if(loadMode == ObjModel::LoadMode::VERTEX_COPY)
                    {
                        auto& uniqIndex = matNameResult.first->second;
                        const auto indexKeyResult = uniqIndex.indexMap.emplace(std::to_string(v), uniqIndex.currIndex);

                        if(indexKeyResult.second)
                        {
                            ++uniqIndex.currIndex;

                            (*mesh.vertices).insert((*mesh.vertices).end(),
                                                    (*poolMesh.vertices).begin() + v * mesh.attrSize,
                                                    (*poolMesh.vertices).begin() + (v + 1) * mesh.attrSize);
                        }

                        (*mesh.indices).push_back(indexKeyResult.first->second);
                    }
                }

                currIndex += 3;
            }

            for(auto& batch : group.batches)
            {
                if(loadMode == ObjModel::LoadMode::VERTEX_COPY)
                {
                    (*batch.meshes.back().vertices).shrink_to_fit();
                }

                (*batch.meshes.back().indices).shrink_to_fit();
            }
        }
    }

    bool SaveMtl(const std::string& mtlPath, const std::vector<Material>& materials)
    {
        std::ofstream mtlFile(mtlPath);

        if(!mtlFile)
        {
            std::cerr << "Failed to save " << mtlPath << std::endl;
            return false;
        }

        for(const auto& mat : materials)
        {
            mtlFile << "newmtl " << mat.name << std::endl;

            mtlFile << "Ka " << mat.Ka.x << " "
                             << mat.Ka.y << " "
                             << mat.Ka.z << std::endl;

            mtlFile << "Kd " << mat.Kd.x << " "
                             << mat.Kd.y << " "
                             << mat.Kd.z << std::endl;

            mtlFile << "Ks " << mat.Ks.x << " "
                             << mat.Ks.y << " "
                             << mat.Ks.z << std::endl;

            mtlFile << "Ns " << mat.shininess << std::endl;

            if(!mat.diffMapName.empty())
            {
                mtlFile << "map_Kd " << mat.diffMapName << std::endl;
            }

            if(!mat.specMapName.empty())
            {
                mtlFile << "map_Ks " << mat.specMapName << std::endl;
            }

            if(!mat.normalMapName.empty())
            {
                mtlFile << "map_Bump " << mat.normalMapName << std::endl;
            }

            mtlFile << std::endl;
        }

        mtlFile.close();

        return true;
    }

    int WriteVertices(std::ofstream& modelFile,
                      int attrSize, const std::shared_ptr<std::vector<float>>& vertices)
    {
        if(!(attrSize > 0) || vertices == nullptr)
        {
            return 0;
        }

        const int vertexCount = int((*vertices).size()) / attrSize;

        for(int v = 0; v < vertexCount; ++v)
        {
            modelFile << "v " << (*vertices)[v * attrSize + 0] << " "
                              << (*vertices)[v * attrSize + 1] << " "
                              << (*vertices)[v * attrSize + 2] << std::endl;

            modelFile << "vt " << (*vertices)[v * attrSize + 3] << " "
                               << (*vertices)[v * attrSize + 4] << std::endl;

            modelFile << "vn " << (*vertices)[v * attrSize + 5] << " "
                               << (*vertices)[v * attrSize + 6] << " "
                               << (*vertices)[v * attrSize + 7] << std::endl;
        }

        return vertexCount;
    }

    int WriteIndices(std::ofstream& modelFile,
                     int attrSize, const std::shared_ptr<std::vector<float>>& vertices,
                     uint32_t currIndex, const std::shared_ptr<std::vector<uint32_t>>& indices)
    {
        if(!(attrSize > 0) || vertices == nullptr || indices == nullptr)
        {
            return 0;
        }

        const int indexCount = int((*indices).size());
        const int faceCount = indexCount / 3;

        for(int f = 0; f < faceCount; ++f)
        {
            modelFile << "f";

            for(int i = 0; i < 3; ++i)
            {
                const auto v = (*indices)[f * 3 + i];

                const float flag = (*vertices)[v * attrSize + 8];

                const bool hasUV = ((flag == 3.0f) || (flag == 1.0f));
                const bool hasNormal = ((flag == 3.0f) || (flag == 2.0f));

                const std::string vs = std::to_string(v + currIndex);
                const std::string ts = (hasUV? "/" + vs : (hasNormal? "/" : ""));
                const std::string ns = (hasNormal? "/" + vs : "");

                modelFile << " " << vs << ts << ns;
            }

            modelFile << std::endl;
        }

        return indexCount;
    }
}

bool ObjModel::Load(
        const std::string& modelPath,
        bool computeNormals/* = false */,
        ObjModel::LoadMode loadMode/* = ObjModel::LoadMode::VERTEX_POOL */)
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    std::string err;
    const std::string mtlBaseDir = MtlBaseDir(modelPath);

    const bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, modelPath.c_str(), mtlBaseDir.c_str());

    if(!err.empty())
    {
        std::cerr << err << std::endl;
    }

    if(!ret)
    {
        std::cerr << "Failed to load " << modelPath << std::endl;
        return false;
    }

    FillMaterials(mtlBaseDir, materials, m_materials);
    std::vector<tinyobj::material_t>().swap(materials);

    if(computeNormals)
    {
        ComputeNormals(attrib, shapes);
    }

    FillPoolMesh(attrib, shapes, m_poolMesh);

    {
        tinyobj::attrib_t emptyAttrib;
        std::swap(attrib, emptyAttrib);
    }

    FillObjects(shapes, m_poolMesh, loadMode, m_objects);

    if(loadMode == ObjModel::LoadMode::VERTEX_COPY)
    {
        m_poolMesh.vertices = nullptr;
    }

    m_poolMesh.indices = nullptr;

    return true;
}

void ObjModel::LoadFromMesh(const Mesh& mesh, bool computeNormals/* = false */)
{
    Unload();

    if(mesh.drawMode != GL_TRIANGLES || !(mesh.attrSize > 0) ||
       mesh.vertices == nullptr || mesh.indices == nullptr)
    {
        return;
    }

    const bool isV3 = (mesh.attrDesc.find("V3") == 0);
    const bool isV3T2 = (isV3? (mesh.attrDesc.find("V3_T2") == 0) : false);
    const bool isV3N3 = (isV3? (mesh.attrDesc.find("V3_N3") == 0) : false);
    const bool isV3T2N3 = (isV3T2? (mesh.attrDesc.find("V3_T2_N3") == 0) : false);
    const bool isV3N3T2 = (isV3N3? (mesh.attrDesc.find("V3_N3_T2") == 0) : false);

    if(!(isV3 || isV3T2 || isV3N3 || isV3T2N3 || isV3N3T2))
    {
        return;
    }

    const bool hasUV = (isV3T2 || isV3T2N3 || isV3N3T2);
    const bool hasNormal = (computeNormals || isV3N3 || isV3T2N3 || isV3N3T2);

    const float flag = (hasUV? 1.0f : 0.0f) + (hasNormal? 2.0f : 0.0f);

    const int vertexCount = int((*mesh.vertices).size()) / mesh.attrSize;

    m_objects.emplace_back(ObjModel::Object::Create(""));

    auto& groups = m_objects.back().groups;
    groups.emplace_back(ObjModel::Group::Create(""));

    auto& batches = groups.back().batches;
    batches.emplace_back(ObjModel::Batch::Create(""));

    auto& meshes = batches.back().meshes;
    meshes.emplace_back(Mesh{GL_TRIANGLES, 9, "V3_T2_N3_FLAG1",
                             std::make_shared<std::vector<float>>(),
                             mesh.indices});

    auto& baseMesh = meshes.back();
    (*baseMesh.vertices).reserve(vertexCount * baseMesh.attrSize);

    std::vector<float> vertexData(baseMesh.attrSize, 0.0f);

    vertexData[7] = (computeNormals? 0.0f : 1.0f);
    vertexData[8] = flag;

    for(int v = 0; v < vertexCount; ++v)
    {
        vertexData[0] = (*mesh.vertices)[v * mesh.attrSize + 0];
        vertexData[1] = (*mesh.vertices)[v * mesh.attrSize + 1];
        vertexData[2] = (*mesh.vertices)[v * mesh.attrSize + 2];

        if(isV3T2 || isV3T2N3)
        {
            vertexData[3] = (*mesh.vertices)[v * mesh.attrSize + 3];
            vertexData[4] = (*mesh.vertices)[v * mesh.attrSize + 4];

            if(!computeNormals && isV3T2N3)
            {
                vertexData[5] = (*mesh.vertices)[v * mesh.attrSize + 5];
                vertexData[6] = (*mesh.vertices)[v * mesh.attrSize + 6];
                vertexData[7] = (*mesh.vertices)[v * mesh.attrSize + 7];
            }
        }
        else if(isV3N3 || isV3N3T2)
        {
            if(isV3N3T2)
            {
                vertexData[3] = (*mesh.vertices)[v * mesh.attrSize + 6];
                vertexData[4] = (*mesh.vertices)[v * mesh.attrSize + 7];
            }

            if(!computeNormals)
            {
                vertexData[5] = (*mesh.vertices)[v * mesh.attrSize + 3];
                vertexData[6] = (*mesh.vertices)[v * mesh.attrSize + 4];
                vertexData[7] = (*mesh.vertices)[v * mesh.attrSize + 5];
            }
        }

        (*baseMesh.vertices).insert((*baseMesh.vertices).end(), vertexData.begin(), vertexData.end());
    }

    if(computeNormals)
    {
        ComputeNormals(baseMesh);
    }
}

void ObjModel::Unload()
{
    std::vector<Material>().swap(m_materials);

    m_poolMesh.vertices = nullptr;
    m_poolMesh.indices = nullptr;

    std::vector<Object>().swap(m_objects);
}

bool ObjModel::Save(const std::string& modelPath)
{
    const int materialCount = int(m_materials.size());

    if(materialCount > 0 && !SaveMtl(MtlPath(modelPath), m_materials))
    {
        return false;
    }

    std::ofstream modelFile(modelPath);

    if(!modelFile)
    {
        std::cerr << "Failed to save " << modelPath << std::endl;
        return false;
    }

    if(materialCount > 0)
    {
        modelFile << "mtllib " << MtlBaseName(modelPath) << std::endl << std::endl;
    }

    const bool isPool = IsVertexPool();
    const auto baseMesh = GetBaseMesh();

    if(isPool)
    {
        WriteVertices(modelFile, baseMesh.attrSize, baseMesh.vertices);
        modelFile << std::endl;
    }

    int currIndex = 1;

    for(const auto& object : m_objects)
    {
        if(!object.name.empty())
        {
            modelFile << "o " << object.name << std::endl;
        }

        for(const auto& group : object.groups)
        {
            for(const auto& batch : group.batches)
            {
                for(const auto& mesh : batch.meshes)
                {
                    modelFile << "# <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << std::endl;

                    if(!group.name.empty())
                    {
                        modelFile << "g " << group.name << std::endl;
                    }

                    if(!batch.matName.empty())
                    {
                        modelFile << "usemtl " << batch.matName << std::endl;
                    }

                    const auto& vertices = (isPool? baseMesh.vertices : mesh.vertices);
                    assert(vertices != nullptr);

                    const int vertexCount = (isPool? 0 : WriteVertices(modelFile, mesh.attrSize, vertices));
                    WriteIndices(modelFile, mesh.attrSize, vertices, currIndex, mesh.indices);

                    currIndex += vertexCount;

                    modelFile << "# >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << std::endl;
                }
            }
        }

        modelFile << std::endl;
    }

    modelFile.close();

    return true;
}

std::vector<Material>& ObjModel::GetMaterials()
{
    return m_materials;
}

const std::vector<Material>& ObjModel::GetMaterials() const
{
    return m_materials;
}

std::vector<ObjModel::Object>& ObjModel::GetObjects()
{
    return m_objects;
}

const std::vector<ObjModel::Object>& ObjModel::GetObjects() const
{
    return m_objects;
}

bool ObjModel::IsVertexPool() const
{
    return m_poolMesh.vertices == GetBaseMesh().vertices;
}

Mesh ObjModel::GetBaseMesh() const
{
    Mesh baseMesh{GL_TRIANGLES, 9, "V3_T2_N3_FLAG1", nullptr, nullptr};

    if(!m_objects.empty() &&
       !m_objects[0].groups.empty() &&
       !m_objects[0].groups[0].batches.empty() &&
       !m_objects[0].groups[0].batches[0].meshes.empty())
    {
        baseMesh = m_objects[0].groups[0].batches[0].meshes[0];
    }

    return baseMesh;
}

