// Graphics/Model/Mesh.h
#pragma once

#include <memory>
#include "VertexBuffer.h"
#include "IndexBuffer.h"

class Mesh {
public:

	Mesh();
	Mesh(const Mesh&) = delete;
	~Mesh();

    bool Init(ID3D11Device*, const MeshData&, unsigned int);
    void Bind(ID3D11DeviceContext* context);

public:
    unsigned int GetMaterialIndex() const { return m_materialIndex; }
    unsigned int GetIndexCount() const { return indexBuffer->GetIndexCount(); }
    const MeshData& GetMeshData() const { return m_meshData; }

private:
    std::unique_ptr<VertexBuffer> vertexBuffer;
    std::unique_ptr<IndexBuffer> indexBuffer;
    unsigned int m_materialIndex;
    MeshData m_meshData;
}; // Mesh