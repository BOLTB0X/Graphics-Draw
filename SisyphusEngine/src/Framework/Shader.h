#pragma once
#include <d3d11.h>
#include <directxmath.h>
#include <wrl/client.h>
#include <string>

enum class ShaderType { None, Sun, Cloud, Bicubic, Sky };

class Shader {
public:
    Shader();
	Shader(const Shader&) = delete;

public:
    // virtual
    virtual ~Shader() = default;
    virtual bool Init(ID3D11Device*, HWND, const std::wstring&, const std::wstring&) = 0;
    virtual void SetShaders(ID3D11DeviceContext*) = 0;

    // common
    bool UpdateMatrixBuffer(ID3D11DeviceContext*, DirectX::XMMATRIX, DirectX::XMMATRIX, DirectX::XMMATRIX);
    bool UpdateGlobalBuffer(ID3D11DeviceContext*, float, float, DirectX::XMFLOAT3, float);
    ShaderType GetShaderType() const { return m_type; }

protected:
    bool Compile(ID3D11Device*, HWND , const std::wstring&, LPCSTR, LPCSTR, ID3DBlob**);
    void OutputError(ID3DBlob*, HWND, const std::wstring&);

protected:
    struct GlobalBuffer {
        // 1
        float iTime;                    // 4 bytes
        float iFrame;                   // 4 bytes
        DirectX::XMFLOAT2 padding;      // 8 bytes

        // 2
        DirectX::XMFLOAT3 iCameraPos;   // 12 bytes
        float padding1;                 // 4 bytes

        // 3
        DirectX::XMFLOAT2 iResolution;  // 8 bytes
        float iNoiseRes;                // 4 bytes
        float padding2;                 // 4 bytes
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
    
    ShaderType m_type;
}; // Shader
