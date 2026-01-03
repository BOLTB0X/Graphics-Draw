// src/Graphics/Resources/Texture/TextureLoader.h
#pragma once
#include <d3d11.h>
#include <string>

class TextureLoader {
public:

    static bool CreateTextureFromFile(
        ID3D11Device* device,
        ID3D11DeviceContext* context,
        const std::string& filename,
        ID3D11ShaderResourceView** outSRV
    );
}; // TextureLoader