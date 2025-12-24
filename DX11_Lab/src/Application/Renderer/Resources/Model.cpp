#include "Renderer/Resources/Model.h"
#include "Renderer/Resources/AssimpLoader.h"
#include <filesystem>

Model::Model() {}

Model::~Model() {
    for (auto& pair : mTextureCache) {
        if (pair.second) {
            pair.second->Shutdown();
            delete pair.second;
        }
    }
    mTextureCache.clear();
    mEntries.clear();
} // ~Model

bool Model::Load(ID3D11Device* device, const std::string& path)
{
    std::vector<MeshData> meshDatas;
    if (!AssimpLoader::Load(path, meshDatas))
        return false;

    std::string directory = std::filesystem::path(path).parent_path().string();
    if (!directory.empty()) directory += "/";

    for (auto& data : meshDatas)
    {
        MeshEntry entry;
        entry.mesh.Init(device, data.vertices, data.indices);

        if (!data.texturePath.empty()) {
            std::string fullPath = directory + data.texturePath;
            if (mTextureCache.find(fullPath) == mTextureCache.end()) {
                Texture* newTex = new Texture();
                if (newTex->Init(device, fullPath)) {
                    mTextureCache[fullPath] = newTex;
                }
                else {
                    delete newTex;
                    newTex = nullptr;
                }
            }
            entry.texture = mTextureCache[fullPath];
        }

        mEntries.push_back(entry);
    }

    return true;
} // Load

void Model::Draw(ID3D11DeviceContext* context, TextureShader* shader,
    const DirectX::XMMATRIX& world, const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& proj)
{
    if (!context || !shader) return;

    for (auto& entry : mEntries)
    {
        if (entry.mesh.GetIndexCount() == 0) continue;

        ID3D11ShaderResourceView* srv = nullptr;
        if (entry.texture) {
            srv = entry.texture->GetTextureView();
        }

        entry.mesh.Bind(context);
        shader->Render(context, entry.mesh.GetIndexCount(), world, view, proj, srv);
    }
} // Draw