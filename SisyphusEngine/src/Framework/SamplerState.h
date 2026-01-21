#pragma once
#include <d3d11.h>
#include <wrl/client.h>


class SamplerState {
public:
    SamplerState();
    SamplerState(const SamplerState&) = delete;
    ~SamplerState();

    bool Init(ID3D11Device* device);

    void VSSetSamplers(ID3D11DeviceContext* context, UINT slot = 0);
    void PSSetSamplers(ID3D11DeviceContext* context, UINT slot = 0);

private:
    Microsoft::WRL::ComPtr<ID3D11SamplerState> m_samplerState;
};