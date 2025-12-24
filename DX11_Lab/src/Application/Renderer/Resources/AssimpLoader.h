#pragma once
#include <string>
#include <vector>
#include "Renderer/Resources/VertexTypes.h"
#include "Renderer/Resources//Mesh.h"

struct MeshData
{
    std::vector<VertexPN> vertices;
    std::vector<uint32_t> indices;
    std::string texturePath;
};

class AssimpLoader
{
public:
    static bool Load(
        const std::string& path,
        std::vector<MeshData>& outMeshes
    );
};
