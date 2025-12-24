#pragma once
#include <d3d11.h>
#include <string>

class Texture {
public:
    Texture() = default;
    ~Texture();

    bool Init(ID3D11Device* device, const std::string& filename);

    void Shutdown();

    ID3D11ShaderResourceView* GetTextureView() const { 
        return m_textureView; 
    }

private:
    ID3D11ShaderResourceView* m_textureView = nullptr;
};