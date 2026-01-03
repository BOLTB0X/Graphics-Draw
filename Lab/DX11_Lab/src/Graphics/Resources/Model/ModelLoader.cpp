#include "ModelLoader.h"


bool ModelLoader::Load(const std::string& filename, MeshData& outData) {
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(filename,
        aiProcess_Triangulate |
        aiProcess_ConvertToLeftHanded |
        aiProcess_CalcTangentSpace |
        aiProcess_GenSmoothNormals |
        aiProcess_JoinIdenticalVertices
    );

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        return false;
    }

    // 1. 첫 번째 메쉬 데이터 추출
    if (scene->mNumMeshes > 0) {
        aiMesh* mesh = scene->mMeshes[0];
        ProcessMesh(mesh, scene, outData);

        // 2. 해당 메쉬의 재질로부터 텍스처 정보 추출
        ProcessMaterials(scene, mesh->mMaterialIndex, outData);
    }

    return true;
} // Load


void ModelLoader::ProcessMesh(aiMesh* mesh, const aiScene* scene, MeshData& outData) {
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        VertexType vertex;

        // Position
        vertex.position = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };

        // UV (Texture Coordinates)
        if (mesh->mTextureCoords[0]) {
            vertex.texture = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };
        }
        else {
            vertex.texture = { 0.0f, 0.0f };
        }

        // Normal
        vertex.normal = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };

        // Tangent & Binormal
        if (mesh->mTangents) {
            vertex.tangent = { mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z };
            vertex.binormal = { mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z };
        }

        outData.vertices.push_back(vertex);
    }

    // Indices (Faces)
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            outData.indices.push_back(face.mIndices[j]);
        }
    }
} // ProcessMesh


void ModelLoader::ProcessMaterials(const aiScene* scene, unsigned int materialIndex, MeshData& outData) {
    if (materialIndex < 0) return;

    aiMaterial* material = scene->mMaterials[materialIndex];

    aiTextureType types[] = {
        aiTextureType_DIFFUSE,  // 보통 index 0
        aiTextureType_NORMALS,  // 보통 index 1
        aiTextureType_SPECULAR, // 보통 index 2
        aiTextureType_OPACITY   // 투과
    };

    for (aiTextureType type : types) {
        unsigned int count = material->GetTextureCount(type);
        for (unsigned int i = 0; i < count; i++) {
            aiString str;
            if (material->GetTexture(type, i, &str) == AI_SUCCESS) {
                outData.texturePaths.push_back(str.C_Str());
            }
        }
    }
    return;
} // ProcessMaterials