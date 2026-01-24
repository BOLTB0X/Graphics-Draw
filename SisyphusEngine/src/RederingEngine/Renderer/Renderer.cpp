#include "Renderer.h"
#include "DisplayInfo.h"
#include "DX11Device.h"
#include "RenderTarget.h"
#include "Rasterizer.h"
#include "DepthStencilState.h"
#include "BlendState.h"
#include "SamplerState.h"
// Common
#include "EngineSettings.h"


Renderer::Renderer()
    : m_vsync_enabled(false)
{
    m_DisplayInfo = std::make_unique<DisplayInfo>();
    m_DX11Device = std::make_unique<DX11Device>();
    m_RenderTarget = std::make_unique<RenderTarget>();

    m_Rasterizer = std::make_unique<Rasterizer>();
    m_DepthStencilState = std::make_unique<DepthStencilState>();
    m_BlendState = std::make_unique<BlendState>();
    m_SamplerState = std::make_unique<SamplerState>();
} // Renderer

Renderer::~Renderer() {}


bool Renderer::Init(HWND hwnd, bool vsync)
{
    m_vsync_enabled = vsync;

    // 장치 생성
    if (m_DisplayInfo->Init(
        EngineSettings::SCREEN_WIDTH,
        EngineSettings::SCREEN_HEIGHT) == false) return false;

    // DisplayInfo의 정보 주입 및 스왑체인 생성
    if (m_DX11Device->Init(hwnd,
        EngineSettings::SCREEN_WIDTH,
        EngineSettings::SCREEN_HEIGHT,
        EngineSettings::FULL_SCREEN,
        vsync,
        m_DisplayInfo->GetNumerator(),
        m_DisplayInfo->GetDenominator())
        == false)
        return false;

    // 렌더 타겟뷰 생성 
    if (m_RenderTarget->Init(
        m_DX11Device->GetDevice(),
        m_DX11Device->GetSwapChain(),
        EngineSettings::SCREEN_WIDTH,
        EngineSettings::SCREEN_HEIGHT)
        == false)
        return false;

    // 각종 파이프라인 상태 객체 초기화
    ID3D11Device* device = m_DX11Device->GetDevice();
    if (m_Rasterizer->Init(device) == false)
        return false;
    if (m_DepthStencilState->Init(device) == false)
        return false;
    if (m_BlendState->Init(device) == false)
        return false;
    if (m_SamplerState->Init(device)
        == false) return false;
    return true;
} // Init


void Renderer::BeginScene(float r, float g, float b, float a)
{
    ID3D11DeviceContext* context = m_DX11Device->GetDeviceContext();

    m_RenderTarget->Clear(context, r, g, b, a);

} // BeginScene


void Renderer::EndScene()
{
    bool vsync = EngineSettings::VSYNC_ENABLED;
    if (vsync)
    {
        m_DX11Device->GetSwapChain()->Present(1, 0);
    }
    else
    {
        m_DX11Device->GetSwapChain()->Present(0, 0);
    }
} // EndScene


void Renderer::Shutdown()
{
    if (m_SamplerState) m_SamplerState.reset();
    if (m_BlendState) m_BlendState.reset();
    if (m_DepthStencilState) m_DepthStencilState.reset();
    if (m_Rasterizer) m_Rasterizer.reset();
    if (m_RenderTarget) m_RenderTarget.reset();
    if (m_DX11Device) m_DX11Device.reset();
    if (m_DisplayInfo) m_DisplayInfo.reset();
} // Shutdown


ID3D11Device* Renderer::GetDevice() const
{
    return m_DX11Device->GetDevice();
} // GetDevice

ID3D11DeviceContext* Renderer::GetDeviceContext() const
{
    return m_DX11Device->GetDeviceContext();
} // GetDeviceContext


void Renderer::SetMode(bool enable, bool back)
{
    if (enable)
        m_Rasterizer->SetWireframeState(m_DX11Device->GetDeviceContext());
    else
    {
        if (back)
            m_Rasterizer->SetSolidState(m_DX11Device->GetDeviceContext());
        else
            m_Rasterizer->SetNoCullingState(m_DX11Device->GetDeviceContext());
    }
} // SetMode


void Renderer::SetAlphaBlending(bool enable)
{
    m_BlendState->Bind(m_DX11Device->GetDeviceContext(), enable);
} // SetAlphaBlending


void Renderer::SetDepthBuffer(bool enable)
{
    m_DepthStencilState->Bind(m_DX11Device->GetDeviceContext(), enable);
} // SetDepthBuffer


void Renderer::SetBackBufferRenderTarget()
{
    ID3D11DeviceContext* context = m_DX11Device->GetDeviceContext();
    ID3D11RenderTargetView* rtv = m_RenderTarget->GetRenderTargetView();
    ID3D11DepthStencilView* dsv = m_RenderTarget->GetDepthStencilView();

    context->OMSetRenderTargets(1, &rtv, dsv);

} // SetBackBufferRenderTarget


void Renderer::SetSampler(UINT slot)
{
    m_SamplerState->VSSetSamplers(m_DX11Device->GetDeviceContext(), slot);
    m_SamplerState->PSSetSamplers(m_DX11Device->GetDeviceContext(), slot);
} // SetSampler