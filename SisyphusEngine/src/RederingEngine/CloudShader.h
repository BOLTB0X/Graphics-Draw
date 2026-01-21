#pragma once
// Framework
#include "Shader.h"


class CloudShader : public Shader {
public:
    CloudShader();
    CloudShader(const CloudShader& other) = delete;
    virtual ~CloudShader() = default;

    virtual bool Init(ID3D11Device*, HWND,
        const std::wstring&, const std::wstring&) override;

public:
    void BindShader(ID3D11DeviceContext* context);
    void BindConstantBuffers(ID3D11DeviceContext* context);

    bool UpdateMatrixBuffer(ID3D11DeviceContext*, DirectX::XMMATRIX, DirectX::XMMATRIX, DirectX::XMMATRIX);
    bool UpdateGlobalBuffer(ID3D11DeviceContext*, float, float);

}; // CloudShader