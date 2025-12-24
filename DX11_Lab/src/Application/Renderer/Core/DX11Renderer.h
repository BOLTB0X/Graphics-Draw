#pragma once
#include <windows.h>
#include <d3d11.h>
#include <wrl.h>

class DX11Renderer
{
public:
    ~DX11Renderer();
    bool Init(HWND hwnd);
    void BeginFrame();
    void EndFrame();
    void Shutdown();

    ID3D11Device* GetDevice() { return m_Device.Get(); }
    ID3D11DeviceContext* GetContext() { return m_Context.Get(); }

private:
    Microsoft::WRL::ComPtr<ID3D11Device> m_Device;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_Context;
    Microsoft::WRL::ComPtr<IDXGISwapChain> m_SwapChain;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_RTV;
};