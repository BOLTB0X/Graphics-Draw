#pragma once
#include <d3d11.h>
#include <directxmath.h>
#include <wrl/client.h> // Microsoft::WRL::ComPtr 사용을 위해

class Renderer {
public:
    Renderer();
    ~Renderer();

    bool Init(HWND hwnd, bool vsync);
    void Shutdown();

    void BeginScene(float r, float g, float b, float a);
    void EndScene();

    void SetAlphaBlending(bool enable);
    void SetZBuffer(bool enable);
    void SetWireframe(bool enable);

    // Getters: 다른 클래스(Shader 등)에서 장치에 접근할 수 있도록 함
    ID3D11Device* GetDevice() const { return m_device.Get(); }
    ID3D11DeviceContext* GetDeviceContext() const { return m_deviceContext.Get(); }

private:
    bool m_vsync_enabled;

    Microsoft::WRL::ComPtr<ID3D11Device> m_device;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_deviceContext;
    Microsoft::WRL::ComPtr<IDXGISwapChain> m_swapChain;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_renderTargetView;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> m_depthStencilBuffer;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_depthStencilView;

    // 파이프라인 상태 객체들
    Microsoft::WRL::ComPtr<ID3D11BlendState> m_alphaEnableBlendingState;
    Microsoft::WRL::ComPtr<ID3D11BlendState> m_alphaDisableBlendingState;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_depthEnableState;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_depthDisableState;
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_rasterState;
}; // Renderer