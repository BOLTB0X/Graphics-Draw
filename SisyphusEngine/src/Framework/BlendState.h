#pragma once
#include <d3d11.h>
#include <wrl/client.h>


// 투명도 처리를 켠 상태(AlphaBlend)와 끈 상태(Opaque)를 관리
class BlendState {
public:
    BlendState();
	BlendState(const BlendState&) = delete;
    ~BlendState();

    bool Init(ID3D11Device*);
    bool InitForParticle(ID3D11Device* device);
    void Bind(ID3D11DeviceContext* context, bool alphaBlend = false);

private:
    Microsoft::WRL::ComPtr<ID3D11BlendState> m_alphaEnableState;
    Microsoft::WRL::ComPtr<ID3D11BlendState> m_alphaDisableState;
    Microsoft::WRL::ComPtr<ID3D11BlendState> m_alphaToCoverageState;
}; // BlendState