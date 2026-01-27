#include "Pch.h"
#include "RenderTarget.h"
// Common
#include "DebugHelper.h"


using namespace Microsoft::WRL;
using namespace DebugHelper;


RenderTarget::RenderTarget()
{ 
    m_viewport = { 0 };
} // RenderTarget


RenderTarget::~RenderTarget()
{} // ~RenderTarget


bool RenderTarget::Init(ID3D11Device* device, IDXGISwapChain* swapChain, int width, int height)
{
    if (InitDethStencil(device, swapChain, width, height) == false)
        return false;

    InitViewport(width, height);
    
    return true;
} // Init


bool RenderTarget::Init(ID3D11Device* device, int width, int height)
{
    if (InitLowResolution(device, width / 2, height / 2) == false)
        return false;

    InitViewport(width, height);
    return true;
} // Init


void RenderTarget::Clear(ID3D11DeviceContext* context, float r, float g, float b, float a) {
    float color[4] = { r, g, b, a };

    // 화면과 깊이 버퍼 지우기
    context->ClearRenderTargetView(m_renderTargetView.Get(), color);
    context->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    // 렌더 타겟 바인딩 및 뷰포트 설정
    context->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), m_depthStencilView.Get());
    context->RSSetViewports(1, &m_viewport);
} // Clear


bool RenderTarget::InitDethStencil(ID3D11Device* device, IDXGISwapChain* swapChain, int width, int height)
{
    ComPtr<ID3D11Texture2D> backBufferPtr;
    if (SuccessCheck(swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D),
        &backBufferPtr),
        "스왑 체인에서 백 버퍼 획득 실패")
        == false) return false;

    if (SuccessCheck(device->CreateRenderTargetView(backBufferPtr.Get(),
        nullptr, &m_renderTargetView),
        "RenderTargetView 생성 실패") == false) return false;

    // DepthStencilBuffer (2D 텍스처) 설정 및 생성
    D3D11_TEXTURE2D_DESC depthBufferDesc = {};
    ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

    depthBufferDesc.Width = width;
    depthBufferDesc.Height = height;
    depthBufferDesc.MipLevels = 1;
    depthBufferDesc.ArraySize = 1;
    depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthBufferDesc.SampleDesc.Count = 1;
    depthBufferDesc.SampleDesc.Quality = 0;
    depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthBufferDesc.CPUAccessFlags = 0;
    depthBufferDesc.MiscFlags = 0;

    // 깊이 버퍼용 텍스처를 생성
    if (SuccessCheck(
        device->CreateTexture2D(&depthBufferDesc, nullptr, &m_depthStencilBuffer),
        "깊이 버퍼 텍스처 생성 실패")
        == false) return false;

    // DepthStencilView 생성
    D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};
    ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

    depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    depthStencilViewDesc.Texture2D.MipSlice = 0;

    if (SuccessCheck(device->CreateDepthStencilView(m_depthStencilBuffer.Get(), &depthStencilViewDesc, &m_depthStencilView),
        "DepthStencilView 생성 실패") == false) return false;
    return true;
} // InitDethStencil


bool RenderTarget::InitLowResolution(ID3D11Device* device, int width, int height)
{
    D3D11_TEXTURE2D_DESC texDesc = {};
    texDesc.Width = width;
    texDesc.Height = height;
    texDesc.MipLevels = 1;
    texDesc.ArraySize = 1;
    texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    texDesc.SampleDesc.Count = 1;
    texDesc.Usage = D3D11_USAGE_DEFAULT;
    texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

    ComPtr<ID3D11Texture2D> renderTexture;
    if (SuccessCheck(device->CreateTexture2D(&texDesc, nullptr, &renderTexture),
        "InitLowResolution: CreateTexture2D 실패") == false)
        return false;
    if (SuccessCheck(device->CreateRenderTargetView(renderTexture.Get(), nullptr, &m_renderTargetView),
        "InitLowResolution: CreateRenderTargetView") == false)
        return false;
    if (SuccessCheck(device->CreateShaderResourceView(renderTexture.Get(), nullptr, &m_shaderResourceView),
        "InitLowResolution: CreateShaderResourceView") == false)
        return false;

    return true;
} // InitLowResolution


void RenderTarget::InitViewport(int width, int height)
{
    m_viewport.Width = (float)width;
    m_viewport.Height = (float)height;
    m_viewport.MinDepth = 0.0f;
    m_viewport.MaxDepth = 1.0f;
    m_viewport.TopLeftX = 0.0f;
    m_viewport.TopLeftY = 0.0f;
} // InitViewport