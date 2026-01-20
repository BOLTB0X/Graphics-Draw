#pragma once
#include <directxmath.h>
#include <vector>

struct MeshVertex {
    DirectX::XMFLOAT3 position;
    DirectX::XMFLOAT2 texture;
    DirectX::XMFLOAT3 normal;
    DirectX::XMFLOAT3 tangent;
    DirectX::XMFLOAT3 binormal;
}; // ModelVertex


struct MeshData {
    std::vector<MeshVertex> vertices;
    std::vector<unsigned int> indices;
}; // MeshData