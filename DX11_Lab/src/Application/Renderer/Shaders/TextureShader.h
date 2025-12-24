#pragma once
#include "Renderer/Shaders/Shader.h"

class TextureShader : public Shader {
public:
    TextureShader() {};
    virtual ~TextureShader() override;

    // Shader
    virtual bool Init(ID3D11Device* device, HWND hwnd,
        const std::wstring& vsPath, const std::wstring& psPath) override;
    
    virtual bool Render(ID3D11DeviceContext* context, int indexCount,
        const DirectX::XMMATRIX& world,
        const DirectX::XMMATRIX& view,
        const DirectX::XMMATRIX& projection) override {
        return Render(context, indexCount, world, view, projection, nullptr);
    } // Render

    // Shader
    bool Render(ID3D11DeviceContext* context, int indexCount,
        const DirectX::XMMATRIX& world,
        const DirectX::XMMATRIX& view,
        const DirectX::XMMATRIX& projection,
        ID3D11ShaderResourceView* textureView);

private:
    bool SetShaderParameters(ID3D11DeviceContext* context,
        const DirectX::XMMATRIX& world,
        const DirectX::XMMATRIX& view,
        const DirectX::XMMATRIX& projection,
        ID3D11ShaderResourceView* textureView);

    ID3D11SamplerState* m_sampleState = nullptr;
};