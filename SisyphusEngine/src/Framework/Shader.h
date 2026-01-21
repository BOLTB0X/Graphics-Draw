#pragma once
#include <d3d11.h>
#include <directxmath.h>
#include <wrl/client.h>
#include <string>


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
        // 1
        float uTime;
        DirectX::XMFLOAT3 padding;

        // 2
        DirectX::XMFLOAT2 uResolution;
        float uNoiseRes;
        float padding2;
    };

    struct MatrixBuffer
    {
        DirectX::XMMATRIX model;
        DirectX::XMMATRIX view;
        DirectX::XMMATRIX projection;
    };

protected:
    Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShader;
    Microsoft::WRL::ComPtr<ID3D11InputLayout> m_layout;

    // 상수버퍼
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_matrixBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_globalBuffer;
}; // Shader
