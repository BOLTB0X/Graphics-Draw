#pragma once
#include <d3d11.h>
#include <directxmath.h>
#include <wrl/client.h>
#include <string>


using namespace DirectX;
using Microsoft::WRL::ComPtr;

class Shader {
public:
    Shader();
	Shader(const Shader&) = delete;

public: // virtual
    virtual ~Shader() = default;
    virtual bool Init(ID3D11Device*, HWND, const std::wstring&, const std::wstring&) = 0;

protected:
    bool Compile(ID3D11Device*, HWND , const std::wstring&, LPCSTR, LPCSTR, ID3DBlob**);
    void OutputError(ID3DBlob*, HWND, const std::wstring&);

protected:
    struct GlobalBuffer {
        float uTime;
        XMFLOAT3 padding;
        XMFLOAT2 uResolution;
        XMFLOAT2 padding2;
    };

    struct MatrixBuffer
    {
        XMMATRIX world;
        XMMATRIX view;
        XMMATRIX projection;
    };

protected:
    ComPtr<ID3D11VertexShader> m_vertexShader;
    ComPtr<ID3D11PixelShader> m_pixelShader;
    ComPtr<ID3D11InputLayout> m_layout;

    // 상수버퍼
    ComPtr<ID3D11Buffer> m_matrixBuffer;
    ComPtr<ID3D11Buffer> m_globalBuffer;
}; // Shader
