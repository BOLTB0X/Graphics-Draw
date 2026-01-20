#include "Renderer.h"
// Common
#include "EngineSettings.h"


Renderer::Renderer()
    : m_vsync_enabled(false)
{}

Renderer::~Renderer() {}


bool Renderer::Init(HWND hwnd, bool vsync)
{
    m_vsync_enabled = vsync;

    // Swap Chain 설정
    DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
    swapChainDesc.BufferCount = 1;
    swapChainDesc.BufferDesc.Width = EngineSettings::SCREEN_WIDTH;
    swapChainDesc.BufferDesc.Height = EngineSettings::SCREEN_HEIGHT;
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.OutputWindow = hwnd;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.Windowed = !EngineSettings::FULL_SCREEN;

    // Device & SwapChain 생성
    UINT creationFlags = 0;
#ifdef _DEBUG
    creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    HRESULT hr = D3D11CreateDeviceAndSwapChain(
        nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, creationFlags,
        nullptr, 0, D3D11_SDK_VERSION, &swapChainDesc,
        &m_swapChain, &m_device, nullptr, &m_deviceContext
    );
    if (FAILED(hr)) return false;

    // RenderTargetView 생성
    Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
    m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), &backBuffer);
    m_device->CreateRenderTargetView(backBuffer.Get(), nullptr, &m_renderTargetView);

    // Depth Stencil Buffer & View 생성
    D3D11_TEXTURE2D_DESC depthBufferDesc = {};
    depthBufferDesc.Width = EngineSettings::SCREEN_WIDTH;
    depthBufferDesc.Height = EngineSettings::SCREEN_HEIGHT;
    depthBufferDesc.MipLevels = 1;
    depthBufferDesc.ArraySize = 1;
    depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthBufferDesc.SampleDesc.Count = 1;
    depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

    m_device->CreateTexture2D(&depthBufferDesc, nullptr, &m_depthStencilBuffer);
    m_device->CreateDepthStencilView(m_depthStencilBuffer.Get(), nullptr, &m_depthStencilView);

    // Blend/Depth 파이프라인 상태 설정
    D3D11_BLEND_DESC blendDesc = {};
    blendDesc.RenderTarget[0].BlendEnable = TRUE;
    blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
    blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    m_device->CreateBlendState(&blendDesc, &m_alphaEnableBlendingState);

    D3D11_DEPTH_STENCIL_DESC depthDesc = {};
    depthDesc.DepthEnable = TRUE;
    depthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthDesc.DepthFunc = D3D11_COMPARISON_LESS;
    m_device->CreateDepthStencilState(&depthDesc, &m_depthEnableState);

    // 뷰포트 설정
    D3D11_VIEWPORT viewport = {};
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = (float)EngineSettings::SCREEN_WIDTH;
    viewport.Height = (float)EngineSettings::SCREEN_HEIGHT;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    m_deviceContext->RSSetViewports(1, &viewport);

    return true;
} // Init


void Renderer::BeginScene(float r, float g, float b, float a)
{
    float color[4] = { r, g, b, a };
    m_deviceContext->ClearRenderTargetView(m_renderTargetView.Get(), color);
    m_deviceContext->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), m_depthStencilView.Get());
} // BeginScene


void Renderer::EndScene()
{
    m_swapChain->Present(EngineSettings::VSYNC_ENABLED ? 1 : 0, 0);
} // EndScene


void Renderer::Shutdown()
{
    if (m_swapChain) m_swapChain->SetFullscreenState(FALSE, nullptr);
} // Shutdown