#pragma once
#include <vector>
#include <string>
#include "Renderer/Resources/Texture.h"
#include "Renderer/Resources/Mesh.h"
#include "Renderer/Shaders/TextureShader.h"
#include <map>

class Model {
public:
	Model();
	~Model();

    bool Load(ID3D11Device* device, const std::string& path);
    void Draw(ID3D11DeviceContext* context, TextureShader* shader,
        const DirectX::XMMATRIX& world, const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& proj);

private:
    struct MeshEntry {
        Mesh mesh;
        Texture* texture = nullptr;
    };
    std::vector<MeshEntry> mEntries;
    std::map<std::string, Texture*> mTextureCache;
};