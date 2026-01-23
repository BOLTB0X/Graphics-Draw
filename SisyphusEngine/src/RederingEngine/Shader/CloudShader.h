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
    virtual void SetShaders(ID3D11DeviceContext*) override;

public:
    void SetConstantBuffers(ID3D11DeviceContext*, ID3D11Buffer*);

}; // CloudShader