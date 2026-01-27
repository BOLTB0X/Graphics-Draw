#pragma once
#include <d3d11.h>
#include <wrl/client.h>


// 렌더링의 "View"와 "깊이 기록", "저해상" 들을 관리
// RTV 및 DSV 생성, 뷰포트 설정
class RenderTarget {
public:
    RenderTarget();
	RenderTarget(const RenderTarget&) = delete;
    ~RenderTarget();

    bool Init(ID3D11Device*, IDXGISwapChain*, int, int);
    bool Init(ID3D11Device*, int, int);
    void Clear(ID3D11DeviceContext*, float, float, float, float);

public:
    ID3D11RenderTargetView* GetRenderTargetView() const { return m_renderTargetView.Get(); }
    ID3D11ShaderResourceView* GetSRV() const { return m_shaderResourceView.Get(); }
    ID3D11Texture2D* GettDepthStencilBuffer() const { return m_depthStencilBuffer.Get(); }
    ID3D11DepthStencilView* GetDepthStencilView() const { return m_depthStencilView.Get(); }
    const D3D11_VIEWPORT& GetViewport() const { return m_viewport; }

private:
    bool InitDethStencil(ID3D11Device*, IDXGISwapChain*, int, int);
    bool InitLowResolution(ID3D11Device*, int, int);
    void InitViewport(int, int);

private:
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_renderTargetView;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_shaderResourceView;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> m_depthStencilBuffer;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_depthStencilView;
    D3D11_VIEWPORT m_viewport;
}; // RenderTarget