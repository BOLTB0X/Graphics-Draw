// ModelLoader.h
#ifndef _MODELLOADER_H_
#define _MODELLOADER_H_

#include "Common/CommonType.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

struct MeshData {
    std::vector<VertexType> vertices;
    std::vector<unsigned long> indices;
    std::vector<std::string> texturePaths; // 여러 종류의 텍스처 경로 보관
}; // MeshData


class ModelLoader {
public:
    static bool Load(const std::string& filename, MeshData& outData);

private:
    static void ProcessMesh(aiMesh* mesh, const aiScene* scene, MeshData& outData);
    static void ProcessMaterials(const aiScene* scene, unsigned int materialIndex, MeshData& outData);
}; // ModelLoader

#endif