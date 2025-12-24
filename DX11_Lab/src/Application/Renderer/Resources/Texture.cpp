#include "Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Texture::~Texture() {
    Shutdown();
} // ~Texture

bool Texture::Init(ID3D11Device* device, const std::string& filename) {
    int width, height, channels;

    unsigned char* pixels = stbi_load(filename.c_str(), &width, &height, &channels, STBI_rgb_alpha);
    if (!pixels) {
        return false;
    }

    D3D11_TEXTURE2D_DESC textureDesc{};
    textureDesc.Width = (UINT)width;
    textureDesc.Height = (UINT)height;
    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = 1;
    textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.Usage = D3D11_USAGE_DEFAULT;
    textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

    D3D11_SUBRESOURCE_DATA initData{};
    initData.pSysMem = pixels;
    initData.SysMemPitch = (UINT)(width * 4);

    ID3D11Texture2D* texture2D = nullptr;
    HRESULT hr = device->CreateTexture2D(&textureDesc, &initData, &texture2D);

    if (SUCCEEDED(hr)) {
        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
        srvDesc.Format = textureDesc.Format;
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MipLevels = 1;

        hr = device->CreateShaderResourceView(texture2D, &srvDesc, &m_textureView);
        texture2D->Release();
    }

    stbi_image_free(pixels);

    return SUCCEEDED(hr);
} // Init

void Texture::Shutdown() {
    if (m_textureView) {
        m_textureView->Release();
        m_textureView = nullptr;
    }
} // Shutdown