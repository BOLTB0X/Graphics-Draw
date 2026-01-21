#pragma once
#include <d3d11.h>
#include <wrl/client.h>


// 여러 종류의 래스터라이저 상태 생성
class Rasterizer {
public:
    Rasterizer();
	Rasterizer(const Rasterizer&) = delete;
    ~Rasterizer();

    bool Init(ID3D11Device* device);
    void Bind(ID3D11DeviceContext* context, bool wireframe = false, bool cullNone = false);

private:
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_solidState;
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_wireframeState;
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_noCullingState;
}; // Rasterizer