////////////////////////////////////////////////////////////////////////////////
// Filename: ResourceManager.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _RESOURCEMANAGER_H_
#define _RESOURCEMANAGER_H_


//////////////
// INCLUDES //
//////////////
#include <map>
#include <string>
#include <vector>

#include "Common/CommonType.h"
#include "Model/Model.h"
#include "Texture/Texture.h"

class ResourceManager
{
public:
    ResourceManager();
    ~ResourceManager();

    void Shutdown();

    // 모델 로드 및 가져오기
    bool LoadModel(ID3D11Device* device, char* modelFilename, std::string key);
    Model* GetModel(std::string key);

    // 텍스처 로드 및 가져오기
    bool LoadTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* filename, std::string key);
    ID3D11ShaderResourceView* GetTexture(std::string key);

private:
    // 경로(key)를 기준으로 리소스를 저장하는 캐시
    std::map<std::string, Model*> m_Models;
    std::map<std::string, Texture*> m_Textures;
};

#endif