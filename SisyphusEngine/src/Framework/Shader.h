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
        // Row 1: Time(4) + Frame(4) + Resolution(8) = 16 bytes
        float iTime;
        float iFrame;
        DirectX::XMFLOAT2 iResolution;

        // Row 2: CameraPos(12) + iNoiseRes(4) = 16 bytes
        DirectX::XMFLOAT3 iCameraPos;
        float iNoiseRes;

        // Row 3: iCloudType(4) + Padding(12) = 16 bytes
        float iCloudType;
        DirectX::XMFLOAT3 padding;
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
