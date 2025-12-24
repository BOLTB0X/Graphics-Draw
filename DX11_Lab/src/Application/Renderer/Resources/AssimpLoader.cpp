#include "Renderer/Resources/AssimpLoader.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>

bool AssimpLoader::Load(
    const std::string& path,
    std::vector<MeshData>& outMeshes
)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(
        path.c_str(),
        aiProcess_Triangulate |
        aiProcess_GenNormals |
        aiProcess_JoinIdenticalVertices |
        aiProcess_ConvertToLeftHanded
    );

    // 로드 실패 체크
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cerr << "Assimp 에러: " << importer.GetErrorString() << std::endl;
        return false;
    }

    // 1(버텍스) 2(인덱스) 3(재질)
    for (unsigned int i = 0; i < scene->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[i];
        MeshData data;

        // 1
        for (unsigned int v = 0; v < mesh->mNumVertices; v++)
        {
            VertexPN vert{};
            vert.position = { 
                mesh->mVertices[v].x,
                mesh->mVertices[v].y,
                mesh->mVertices[v].z
            };

            if (mesh->HasTextureCoords(0)) {
                vert.uv = {
                    mesh->mTextureCoords[0][v].x,
                    mesh->mTextureCoords[0][v].y
                };
            }

            if (mesh->HasNormals()) {
                vert.normal = {
                    mesh->mNormals[v].x,
                    mesh->mNormals[v].y,
                    mesh->mNormals[v].z
                };
            }

            data.vertices.push_back(vert);
        } // for v

        // 2
        for (unsigned int f = 0; f < mesh->mNumFaces; f++)
        {
            aiFace& face = mesh->mFaces[f];
            for (unsigned int index = 0; index < face.mNumIndices; index++)
            {
                data.indices.push_back(face.mIndices[index]);
            }
        } //  for f

        // 3
        if (mesh->mMaterialIndex >= 0) {
            aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
            aiString texPath;

            if (material->GetTexture(aiTextureType_DIFFUSE, 0, &texPath) == AI_SUCCESS) {
                data.texturePath = texPath.C_Str();
            }
        }

        outMeshes.push_back(data);
    } // for i

    return true;
} // Load
