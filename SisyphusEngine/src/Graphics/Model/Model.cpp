// Graphics/Model/Model.cpp
#include "Model.h"

/* default */
////////////////////////////////////////////////////////////////////////////

Model::Model() {};

Model::~Model() {};


void Model::Render(ID3D11DeviceContext* context)
{
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    for (const auto& mesh : m_meshes)
    {
        // 해당 메쉬의 머터리얼 인덱스 확인
        unsigned int matIdx = mesh->GetMaterialIndex();

        if (matIdx < m_materials.size())
        {
			const Material& material = m_materials[matIdx];
            //if (material.diffuse) material.diffuse->Bind(context, 0);
            //if (material.ambient) material.ambient->Bind(context, 1);
            //if (material.specular) material.specular->Bind(context, 2);

            if (material.albedo) material.albedo->Bind(context, 0);
            if (material.normal)    material.normal->Bind(context, 1);
            if (material.metallic)  material.metallic->Bind(context, 2);
            if (material.roughness) material.roughness->Bind(context, 3);
            if (material.ao)        material.ao->Bind(context, 4);

        }

        // 메쉬 그리기
        mesh->Bind(context);
        context->DrawIndexed(mesh->GetIndexCount(), 0, 0);
    }
} // Render

void Model::AddMesh(std::unique_ptr<Mesh> mesh)
{
    m_meshes.push_back(std::move(mesh));
} // AddMesh


void Model::AddMaterial(const Material& material)
{
    m_materials.push_back(material);
} // AddMaterial


std::vector<MeshData> Model::GetFullMeshData() const
{
    std::vector<MeshData> allData;
    for (const auto& mesh : m_meshes)
        allData.push_back(mesh->GetMeshData());
    
    return allData;
} // GetFullMeshData