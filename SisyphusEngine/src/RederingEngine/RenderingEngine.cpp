#include "Pch.h"
#include "RenderingEngine.h"
#include "Renderer/Renderer.h"
#include "Model/DefaultModel.h"
#include "Model/Light.h"
#include "Model/TexturesManager.h"
#include "Shader/ShaderManager.h"
// Common
#include "ConstantHelper.h"
#include "DebugHelper.h"
#include "PropertyHelper.h"


using namespace PropertyHelper;
using namespace DirectX;


RenderingEngine::RenderingEngine()
    : m_isWireframe(false),
    m_backCullEnable(false),
    m_depthEnable(true)
{
    m_Renderer = std::make_unique<Renderer>();
    m_TexturesManager = std::make_unique<TexturesManager>();
    m_ShaderManager = std::make_unique<ShaderManager>();
    m_SunModel = std::make_unique<DefaultModel>();
    m_CloudArea = std::make_unique<DefaultModel>();
    m_Light = std::make_unique<Light>();
} // RenderingEngine


RenderingEngine::~RenderingEngine()
{

} // ~RenderingEngine


bool RenderingEngine::Init(HWND hwnd)
{
    if (m_Renderer->Init(hwnd, true) == false)
        return false;

    if (m_TexturesManager->Init(
        m_Renderer->GetDevice(),
        m_Renderer->GetDeviceContext())
        == false) return false;

    if (m_ShaderManager->Init(
        m_Renderer->GetDevice(),
        hwnd) == false) return false;


    if (m_CloudArea->Init(
        m_Renderer->GetDevice(), DefaultModelType::Sphere) == false)
        return false;
    m_CloudArea->SetScale(10.0f);

    m_Light->Init({ 5.0f, 5.0f, 5.0f }, { 1.0f, 0.9f, 0.7f, 1.0f }, 2.5f);

    if (m_SunModel->Init(
        m_Renderer->GetDevice(), DefaultModelType::Sphere)
        == false) return false;

    m_SunModel->SetPosition(m_Light->GetPosition());

    return true;
} // Init


void RenderingEngine::Shutdown()
{
    if (m_ShaderManager)
        m_ShaderManager.reset();

    if (m_TexturesManager)
        m_TexturesManager.reset();

    if (m_Renderer)
    {
        m_Renderer->Shutdown();
        m_Renderer.reset();
    }
    return;
} // Shutdown


void RenderingEngine::Draw(float frameTime,
    Property<XMMATRIX> viewProp,
    Property<XMMATRIX> projProp,
    Property<XMFLOAT3> camPosProp)
{
    ID3D11DeviceContext* context = m_Renderer->GetDeviceContext();

    XMFLOAT3 camPos = camPosProp.Get();
    XMMATRIX view = viewProp.Get();
    XMMATRIX proj = projProp.Get();

    m_Renderer->SetAlphaBlending(true);

    m_ShaderManager->UpdateGlobalBuffer(ShaderKeys::Sun,
        context, frameTime, camPos, 0.0f);
    m_ShaderManager->UpdateMatrixBuffer(ShaderKeys::Sun,
        context, m_SunModel->GetModelMatrix(), view, proj);
    m_ShaderManager->UpdateLightBuffer(ShaderKeys::Sun,
        context, m_Light.get());
    m_ShaderManager->SetShaders(ShaderKeys::Sun, context);
    m_ShaderManager->SetConstantBuffers(ShaderKeys::Sun, context);
    m_SunModel->Render(context);

    m_Renderer->SetSampler(0);
    m_TexturesManager->PSSetShaderResources(context,
        ConstantHelper::NOISE_PATH, 0);

    m_ShaderManager->UpdateGlobalBuffer(ShaderKeys::Cloud,
        context, frameTime, camPos, 256.0f);
    m_ShaderManager->UpdateMatrixBuffer(ShaderKeys::Cloud,
        context,
        m_CloudArea->GetModelMatrix(), view, proj);
    m_ShaderManager->UpdateLightBuffer(ShaderKeys::Cloud,
        context, m_Light.get());

    m_ShaderManager->SetShaders(ShaderKeys::Cloud, context);
    m_ShaderManager->SetConstantBuffers(ShaderKeys::Cloud, context);
    m_CloudArea->Render(context);

    m_Renderer->SetAlphaBlending(false);
} // Render


void RenderingEngine::BeginScene(float r, float g, float b, float a)
{
    m_Renderer->BeginScene(r, g, b, a);
} // BeginScene


void RenderingEngine::EndScene()
{
    m_Renderer->EndScene();
} // EndScene


void RenderingEngine::SetMode(bool isWireframe, bool backCullEnable)
{
    m_Renderer->SetMode(isWireframe, backCullEnable);
} // SetMode


void RenderingEngine::SetDepthBuffer(bool depthEnable)
{
    m_Renderer->SetDepthBuffer(depthEnable);
} // SetDepthBuffer


ID3D11Device* RenderingEngine::GetDevice()
{
    return m_Renderer->GetDevice();
} // GetDevice


ID3D11DeviceContext* RenderingEngine::GetDeviceContext()
{
    return m_Renderer->GetDeviceContext();
} // GetDeviceContext