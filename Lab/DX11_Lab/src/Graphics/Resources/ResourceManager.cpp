////////////////////////////////////////////////////////////////////////////////
// Filename: ResourceManager.cpp
////////////////////////////////////////////////////////////////////////////////
#include "ResourceManager.h"

ResourceManager::ResourceManager() {
} // ResourceManager


ResourceManager::~ResourceManager() {
    Shutdown();
} // ~ResourceManager


void ResourceManager::Shutdown()
{
    // 모델 메모리 해제
    for (auto& pair : m_Models) {
        if (pair.second) {
            pair.second->Shutdown();
            delete pair.second;
        }
    }
    m_Models.clear();

    // 텍스처 메모리 해제
    for (auto& pair : m_Textures) {
        if (pair.second) {
            pair.second->Shutdown();
            delete pair.second;
        }
    }
    m_Textures.clear();
} // Shutdown


bool ResourceManager::LoadModel(ID3D11Device* device, char* modelFilename, std::string key)
{
    // 이미 로드되어 있다면 성공으로 간주
    if (m_Models.count(key) > 0) return true;

    Model* newModel = new Model();
    if (!newModel) return false;

    //bool result = newModel->Init(device, modelFilename);
    //if (!result) {
    //    delete newModel;
    //    return false;
    //}

    m_Models[key] = newModel;
    return true;
} // LoadModel


Model* ResourceManager::GetModel(std::string key)
{
    if (m_Models.find(key) != m_Models.end())
    {
        return m_Models[key];
    }

    return nullptr;
} // GetModel


bool ResourceManager::LoadTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* filename, std::string key)
{
    // 이미 로드된 텍스처인지 확인
    if (m_Textures.find(key) != m_Textures.end())
    {
        return true;
    }

    Texture* newTexture = new Texture;
    if (!newTexture)
    {
        return false;
    }

    bool result = newTexture->Init(device, deviceContext, filename);
    if (!result)
    {
        delete newTexture;
        return false;
    }

    m_Textures[key] = newTexture;
    return true;
} // LoadTexture


ID3D11ShaderResourceView* ResourceManager::GetTexture(std::string key)
{
    if (m_Textures.find(key) != m_Textures.end())
    {
        return m_Textures[key]->GetTexture();
    }

    return nullptr;
} // GetTexture