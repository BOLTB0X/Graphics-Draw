#include "Pch.h"
#include "RenderingEngine.h"
#include "Renderer/Renderer.h"
#include "Model/DefaultModel.h"
#include "Model/Light.h"
#include "Model/TexturesManager.h"
#include "Model/Texture.h"
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
    m_depthEnable(true),
    m_frameCount(0)
{
    m_Renderer = std::make_unique<Renderer>();
    m_TexturesManager = std::make_unique<TexturesManager>();
    m_ShaderManager = std::make_unique<ShaderManager>();
    m_SunModel = std::make_unique<DefaultModel>();
    m_CloudArea = std::make_unique<DefaultModel>();
    m_BicubicMesh = std::make_unique<DefaultModel>();
    m_Sky = std::make_unique<DefaultModel>();
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

    if (m_Sky->Init(
        m_Renderer->GetDevice(), DefaultModelType::Cube) == false)
        return false;

    if (m_BicubicMesh->Init(
        m_Renderer->GetDevice(), DefaultModelType::Quad) == false)
        return false;

    if (m_CloudArea->Init(
        m_Renderer->GetDevice(), DefaultModelType::Sphere) == false)
        return false;
    m_CloudArea->SetScale(1.5f);

    m_Light->Init({ -5.0f, 10.0f, 7.0f }, { 1.0f, 0.9f, 0.7f, 1.0f }, 2.5f);

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


void RenderingEngine::Draw(
    float totalTime,
    Property<XMMATRIX> viewProp,
    Property<XMMATRIX> projProp,
    Property<XMFLOAT3> camPosProp)
{
    ID3D11DeviceContext* context = m_Renderer->GetDeviceContext();
    XMFLOAT3 camPos = camPosProp.Get();
    XMMATRIX view = viewProp.Get();
    XMMATRIX proj = projProp.Get();

    // 태양구름
    m_Renderer->SetLowResolutionRenderTarget();
    m_Renderer->SetAlphaBlending(true);
    m_Renderer->SetDepthBuffer(false);

    // 스카이박스 업데이트
// 카메라를 따라다니도록 Translation 행렬 적용
    XMMATRIX skyModel = XMMatrixTranslation(camPos.x, camPos.y, camPos.z);
    m_ShaderManager->UpdateGlobalBuffer(ShaderKeys::Sky,
        context, totalTime,
        (float)m_frameCount,
        camPos, 256.0f);
    m_ShaderManager->UpdateMatrixBuffer(ShaderKeys::Sky, context, skyModel, view, proj);

    m_ShaderManager->SetShaders(ShaderKeys::Sky, context);
    m_ShaderManager->SetConstantBuffers(ShaderKeys::Sky, context);

    m_Sky->Render(context); // 큐브 메쉬 렌더링
    m_Renderer->SetMode(false, true); // Solid, Back-face Culling

    // 태양
    m_ShaderManager->UpdateGlobalBuffer(ShaderKeys::Sun,
        context, totalTime, (float)m_frameCount, camPos, 0.0f);
    m_ShaderManager->UpdateMatrixBuffer(ShaderKeys::Sun,
        context, m_SunModel->GetModelMatrix(), view, proj);
    m_ShaderManager->UpdateLightBuffer(ShaderKeys::Sun,
        context, m_Light.get());
    m_ShaderManager->SetShaders(ShaderKeys::Sun, context);
    m_ShaderManager->SetConstantBuffers(ShaderKeys::Sun, context);
    m_SunModel->Render(context);

    // 구름
    m_Renderer->SetSampler(0);
    m_TexturesManager->PSSetShaderResources(context,
        ConstantHelper::NOISE_PATH, 0);
    m_TexturesManager->PSSetShaderResources(context,
        ConstantHelper::BLUE_NOISE_PATH, 1);

    m_ShaderManager->UpdateGlobalBuffer(ShaderKeys::Cloud,
        context, totalTime, (float)m_frameCount, camPos, 256.0f);
    m_ShaderManager->UpdateMatrixBuffer(ShaderKeys::Cloud,
        context, m_CloudArea->GetModelMatrix(), view, proj);
    m_ShaderManager->SetShaders(ShaderKeys::Cloud, context);
    m_ShaderManager->SetConstantBuffers(ShaderKeys::Cloud, context);
    m_CloudArea->Render(context);

    m_Renderer->SetBackBufferRenderTarget();

    // 스카이박스 상태 설정
    // 깊이 쓰기 끄기, 컬링 끄기
    //m_Renderer->SetDepthBuffer(false);
    //m_Renderer->SetMode(false, false); // Wireframe: false, Back: false (No Culling)


    // ---  메인 백버퍼: 바이큐빅 합성 (구름 덮기) ---
    m_Renderer->SetAlphaBlending(true);
    m_ShaderManager->SetShaders(ShaderKeys::Bicubic, context);
    m_Renderer->SetLowResolutionShaderResources(0);
    // 바이큐빅 보간을 위한 선형 샘플러
    m_ShaderManager->UpdateMatrixBuffer(
        ShaderKeys::Bicubic, context,
        m_BicubicMesh->GetModelMatrix(), XMMatrixIdentity(), XMMatrixIdentity());
    m_BicubicMesh->Render(context);

    m_Renderer->SetDepthBuffer(true);
    m_Renderer->SetAlphaBlending(false);
    m_frameCount++;
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