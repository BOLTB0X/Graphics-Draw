#pragma once
// Framework
#include "Shader.h"


class BicubicShader : public Shader {
public:
    BicubicShader();
    BicubicShader(const BicubicShader& other) = delete;
    virtual ~BicubicShader() = default;

    virtual bool Init(ID3D11Device*, HWND,
        const std::wstring&, const std::wstring&) override;
    virtual void SetShaders(ID3D11DeviceContext*) override;
}; // BicubicShader