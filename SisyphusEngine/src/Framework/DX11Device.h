#pragma once
#include <d3d11.h>
#include <wrl/client.h>


// 장치, 컨텍스트, 스왑체인 생성
class DX11Device {
public:
    DX11Device();
    DX11Device(const DX11Device&) = delete;
    ~DX11Device();

    bool Init(HWND hwnd, int width, int height, 
        bool fullscreen, bool vsync,
        unsigned int numerator, unsigned int denominator);

public:
    ID3D11Device* GetDevice() const { return m_device.Get(); }
    ID3D11DeviceContext* GetDeviceContext() const { return m_deviceContext.Get(); }
    IDXGISwapChain* GetSwapChain() const { return m_swapChain.Get(); }

private:
    Microsoft::WRL::ComPtr<ID3D11Device> m_device;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_deviceContext;
    Microsoft::WRL::ComPtr<IDXGISwapChain> m_swapChain;
}; // DX11Device