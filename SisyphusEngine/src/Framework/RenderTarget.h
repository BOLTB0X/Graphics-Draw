#pragma once
#include <d3d11.h>
#include <wrl/client.h>


// 렌더링의 "View"와 "깊이 기록"을 관리
// RTV 및 DSV 생성, 뷰포트 설정
class RenderTarget {
public:
    RenderTarget();
	RenderTarget(const RenderTarget&) = delete;
    ~RenderTarget();

    bool Init(ID3D11Device*, IDXGISwapChain*, int, int);
    void Clear(ID3D11DeviceContext*, float, float, float, float);

public:
    ID3D11RenderTargetView* GetRenderTargetView() const { return m_renderTargetView.Get(); }
    ID3D11Texture2D* GettDepthStencilBuffer() const { return m_depthStencilBuffer.Get(); }
    ID3D11DepthStencilView* GetDepthStencilView() const { return m_depthStencilView.Get(); }

private:
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_renderTargetView;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> m_depthStencilBuffer;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_depthStencilView;
    D3D11_VIEWPORT m_viewport;
}; // RenderTarget