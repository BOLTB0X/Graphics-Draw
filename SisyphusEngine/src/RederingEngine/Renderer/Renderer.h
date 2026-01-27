#pragma once
#include <d3d11.h>
#include <directxmath.h>
#include <memory>
#include <wrl/client.h> // Microsoft::WRL::ComPtr 사용을 위해

class DisplayInfo;
class DX11Device;
class RenderTarget;
class Rasterizer;
class DepthStencilState;
class BlendState;
class SamplerState;

class Renderer {
public:
    Renderer();
    Renderer(const Renderer& other) = delete;
    ~Renderer();

    bool Init(HWND hwnd, bool vsync);
    void Shutdown();

    void BeginScene(float r, float g, float b, float a);
    void EndScene();

public:
    void SetMode(bool, bool);
    void SetAlphaBlending(bool);
    void SetDepthBuffer(bool);
    void SetBackBufferRenderTarget();
    void SetSampler(UINT slot);
    void SetLowResolutionRenderTarget();
    void SetLowResolutionShaderResources(UINT);

    ID3D11Device* GetDevice() const;
    ID3D11DeviceContext* GetDeviceContext() const;

private:
    bool m_vsync_enabled;

    // 하드웨어 및 기본 리소스
    std::unique_ptr<DisplayInfo> m_DisplayInfo;
    std::unique_ptr<DX11Device>  m_DX11Device;
    std::unique_ptr<RenderTarget> m_MainRenderTarget;
    std::unique_ptr<RenderTarget> m_LowResRenderTarget;

    // 파이프라인 규칙(States)
    std::unique_ptr<Rasterizer> m_Rasterizer;
    std::unique_ptr<DepthStencilState> m_DepthStencilState;
    std::unique_ptr<BlendState> m_BlendState;
    std::unique_ptr<SamplerState> m_SamplerState;
}; // Renderer