// src/Graphics/Resources/Texture/TextureLoader.cpp
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "TextureLoader.h"

bool TextureLoader::CreateTextureFromFile(
    ID3D11Device* device,
    ID3D11DeviceContext* context,
    const std::string& filename,
    ID3D11ShaderResourceView** outSRV) {
    int width, height, channels;

    unsigned char* imageData = stbi_load(filename.c_str(), &width, &height, &channels, STBI_rgb_alpha);

    if (!imageData) return false;

    // Texture2D 설정
    D3D11_TEXTURE2D_DESC td = {};
    td.Width = width;
    td.Height = height;
    td.MipLevels = 0;
    td.ArraySize = 1;
    td.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    td.SampleDesc.Count = 1;
    td.Usage = D3D11_USAGE_DEFAULT;
    td.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
    td.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

    ID3D11Texture2D* texture = nullptr;
    if (FAILED(device->CreateTexture2D(&td, nullptr, &texture))) {
        stbi_image_free(imageData);
        return false;
    }

    // 데이터 업로드
    unsigned int rowPitch = (width * 4) * sizeof(unsigned char);
    context->UpdateSubresource(texture, 0, NULL, imageData, rowPitch, 0);

    // SRV 생성
    D3D11_SHADER_RESOURCE_VIEW_DESC srvd = {};
    srvd.Format = td.Format;
    srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvd.Texture2D.MostDetailedMip = 0;
    srvd.Texture2D.MipLevels = -1;

    if (FAILED(device->CreateShaderResourceView(texture, &srvd, outSRV))) {
        texture->Release();
        stbi_image_free(imageData);
        return false;
    }

    // Mips 생성 및 정리
    context->GenerateMips(*outSRV);
    texture->Release();
    stbi_image_free(imageData);

    return true;
} // CreateTextureFromFile