// Graphics/Model/Model.h
#pragma once

#include <vector>
#include "Mesh/Mesh.h"
#include "Texture/Material.h"


class Model {
public:
    Model();
	Model(const Model&) = delete;
	~Model();

    void Render(ID3D11DeviceContext*);

public:
    void AddMesh(std::unique_ptr<Mesh>);
    void AddMaterial(const Material&);

public:
    std::vector<MeshData> GetFullMeshData() const;

private:
    std::vector<std::unique_ptr<Mesh>> m_meshes;
    std::vector<Material> m_materials;
}; // Model