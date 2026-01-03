#pragma once
// src/Graphics/Resources/Texture/TextureManager.h
#include <unordered_map>
#include <string>
#include <wrl/client.h>
#include "TextureLoader.h"

using Microsoft::WRL::ComPtr;

class TexturesManager {
private:
    std::unordered_map<std::string, ComPtr<ID3D11ShaderResourceView>> m_textures;

public:
    TexturesManager() = default;
    ~TexturesManager() = default;

    // 초기화 시 개수를 정할 필요가 없음
    bool Init() { return true; }

    // 이름으로 로드 (이미 있으면 기존 것 반환)
    ID3D11ShaderResourceView* Load(ID3D11Device* device, ID3D11DeviceContext* context, const std::string& filename) {
        // 1. 이미 로드된 텍스처인가?
        auto it = m_textures.find(filename);
        if (it != m_textures.end()) {
            return it->second.Get();
        }

        // 2. 새로 로드
        ID3D11ShaderResourceView* srv = nullptr;
        if (TextureLoader::CreateTextureFromFile(device, context, filename, &srv)) {
            m_textures[filename] = srv; // ComPtr이 소유권 관리
            return srv;
        }

        return nullptr;
    }

    void Shutdown() {
        m_textures.clear(); // ComPtr이 알아서 Release 호출
    }
};
