#pragma once
#include <d3d11.h>
#include <unordered_map>
#include <string>
#include <memory>

class Texture;

class TexturesManager {
public:
    TexturesManager();
    TexturesManager(const TexturesManager&) = delete;
    ~TexturesManager();

    bool Init(ID3D11Device*, ID3D11DeviceContext*);
    void Shutdown();

public:
    std::shared_ptr<Texture> GetTexture(ID3D11Device*, ID3D11DeviceContext*, const std::string&);
    void PSSetShaderResources(ID3D11DeviceContext*, const std::string&, UINT slot = 0);

private:
    std::unordered_map<std::string, std::shared_ptr<Texture>> m_Textures;
}; // TexturesManager