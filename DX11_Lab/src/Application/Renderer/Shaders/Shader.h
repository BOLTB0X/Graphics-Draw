#pragma once
#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <string>
#include <iostream>

class Shader {
public:
    Shader() = default;
    virtual ~Shader();

    virtual bool Init(ID3D11Device* device, HWND hwnd,
        const std::wstring& vsPath, const std::wstring& psPath) = 0;

    virtual bool Render(ID3D11DeviceContext* context, int indexCount,
        const DirectX::XMMATRIX& world,
        const DirectX::XMMATRIX& view,
        const DirectX::XMMATRIX& projection) = 0;

protected:
    bool CompileShader(const std::wstring& path, const char* entry, const char* profile, ID3DBlob** blob, HWND hwnd);

    bool CreateMatrixBuffer(ID3D11Device* device);

protected:
    ID3D11VertexShader* m_vertexShader = nullptr;
    ID3D11PixelShader* m_pixelShader = nullptr;
    ID3D11InputLayout* m_layout = nullptr;
    ID3D11Buffer* m_matrixBuffer = nullptr; // WVP 행렬용 상수 버퍼

    // 상수 버퍼 구조체 (HLSL의 cbuffer와 일치해야 함)
    struct MatrixBufferType {
        DirectX::XMMATRIX world;
        DirectX::XMMATRIX view;
        DirectX::XMMATRIX projection;
    };
};