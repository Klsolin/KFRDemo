//
//  ObjModel.h
//  KFRDemo
//
//  Created by klsoline on 2023/09/04.
//

#pragma once

#include <vector>
#include <string>

#include "Material.h"
#include "Mesh.h"

class ObjModel
{
public:
    enum class LoadMode
    {
        VERTEX_POOL,
        VERTEX_COPY
    };

    struct Batch
    {
        int matId;
        std::string matName;
        std::vector<Mesh> meshes;

        using KeyType = decltype(matName);

        const KeyType& Key() const { return matName; }
        static Batch Create(const KeyType& key) { return Batch{-1, key, {}}; }
    };

    struct Group
    {
        std::string name;
        std::vector<Batch> batches;

        using KeyType = decltype(name);

        const KeyType& Key() const { return name; }
        static Group Create(const KeyType& key) { return Group{key, {}}; }
    };

    struct Object
    {
        std::string name;
        std::vector<Group> groups;

        using KeyType = decltype(name);

        const KeyType& Key() const { return name; }
        static Object Create(const KeyType& key) { return Object{key, {}}; }
    };

public:
    bool Load(
            const std::string& modelPath,
            bool computeNormals = false,
            ObjModel::LoadMode loadMode = ObjModel::LoadMode::VERTEX_POOL);

    void LoadFromMesh(const Mesh& mesh, bool computeNormals = false);

    void Unload();

    bool Save(const std::string& modelPath);

    std::vector<Material>& GetMaterials();
    const std::vector<Material>& GetMaterials() const;

    std::vector<ObjModel::Object>& GetObjects();
    const std::vector<ObjModel::Object>& GetObjects() const;

    bool IsVertexPool() const;
    Mesh GetBaseMesh() const;

private:
    std::vector<Material> m_materials;

    Mesh m_poolMesh;
    std::vector<Object> m_objects;
};

