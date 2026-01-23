#include "MainEngine.h"
#include "Timer.h"
#include "Fps.h"
#include "Camera.h"
// System
#include "Input.h"
#include "Gui.h"
// UI
#include "UI.h"
#include "InputManager.h"
// Common
#include "EngineSettings.h"
#include "EngineHelper.h"
// Rendering
#include "Renderer/Renderer.h"
#include "Model/DefaultModel.h"
#include "Model/Light.h"
#include "Model/TexturesManager.h"
#include "Shader/ShaderManager.h"

MainEngine::MainEngine()
    : m_isWireframe(false),
    m_backCullEnable(false),
    m_depthEnable(true)
{
    m_Timer = std::make_unique<Timer>();
    m_Fps = std::make_unique<Fps>();
    m_Renderer = std::make_unique<Renderer>();
    m_Camera = std::make_unique<Camera>();
    m_TexturesManager = std::make_unique<TexturesManager>();
    m_ShaderManager = std::make_unique<ShaderManager>();
    m_SunModel = std::make_unique<DefaultModel>();
    m_CloudArea = std::make_unique<DefaultModel>();
    m_InputManager = std::make_unique<InputManager>();
    m_UI = std::make_unique<UI>();
    m_Light = std::make_unique<Light>();
} // MainEngine


MainEngine::~MainEngine() {}


bool MainEngine::Init(HWND hwnd, std::shared_ptr<Input> input, std::shared_ptr<Gui> gui)
{
    m_Input = input;
    m_Gui = gui;

    if (m_Timer->Init() == false) return false;
    m_Fps->Init();

    if (m_Renderer->Init(hwnd, true) == false) return false;

    m_Camera->SetPosition(0.0f, 0.0f, -6.0f);

    if (m_TexturesManager->Init(
        m_Renderer->GetDevice(),
        m_Renderer->GetDeviceContext())
    == false) return false;

    if (m_ShaderManager->Init(
        m_Renderer->GetDevice(),
        hwnd) == false) return false;


    if (m_CloudArea->Init(m_Renderer->GetDevice(), DefaultModelType::Sphere) == false)
        return false;
    m_CloudArea->SetScale(10.0f);

    m_Light->Init({ 5.0f, 5.0f, 5.0f }, { 1.0f, 0.9f, 0.7f, 1.0f }, 2.5f);
    if (m_SunModel->Init(m_Renderer->GetDevice(), DefaultModelType::Sphere)
        == false) return false;
    m_SunModel->SetPosition(m_Light->GetPosition()); // Light의 위치와 동기화

    if (gui->Init(hwnd,
        m_Renderer->GetDevice(),
        m_Renderer->GetDeviceContext())
        == false) return false;

    if (m_InputManager->Init(input) == false) return false;
    if (m_UI->Init(gui) == false) return false;
    
    m_UI->CreateWidget(
        m_Timer.get(),
        m_Fps.get(),
        m_Camera.get(),
        &m_isWireframe,
        &m_backCullEnable,
        &m_depthEnable);
    return true;
} // Init


void MainEngine::Shutdown()
{

    if (m_Renderer) m_Renderer->Shutdown();
} // Shutdown


bool MainEngine::Frame()
{
    m_Timer->Frame();
    m_Fps->Frame();

    if (m_InputManager->Frame(
        m_Timer->GetFrameTime(),
        m_Camera.get(),
        m_UI->IsCameraLocked())
        == false) return false;;

    UpdateUI();
    //m_InputManager->Frame(deltaTime, m_Camera.get(), m_UI->IsCameraLocked());
    UpdateRenderStates();

    Render();

    return true;
} // Frame


void MainEngine::Render()
{
    m_Renderer->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

    ID3D11DeviceContext* context = m_Renderer->GetDeviceContext();

    m_Renderer->SetAlphaBlending(true);

    m_ShaderManager->UpdateGlobalBuffer(ShaderKeys::Sun,
        context, m_Timer->GetTotalTime(),
        m_Camera->GetPosition(),
        0.0f);
    m_ShaderManager->UpdateMatrixBuffer(ShaderKeys::Sun,
        context, m_SunModel->GetModelMatrix(),
        m_Camera->GetViewMatrix(),
        m_Camera->GetProjectionMatrix());
    m_ShaderManager->UpdateLightBuffer(ShaderKeys::Sun,
        context, m_Light.get());
    m_ShaderManager->SetShaders(ShaderKeys::Sun, context);
    m_ShaderManager->SetConstantBuffers(ShaderKeys::Sun, context);
    m_SunModel->Render(context);

    m_Renderer->SetSampler(0);
    m_TexturesManager->PSSetShaderResources(context, EngineSettings::NOISE_PATH, 0);

    m_ShaderManager->UpdateGlobalBuffer(ShaderKeys::Cloud,
        context, m_Timer->GetTotalTime(),
        m_Camera->GetPosition(),
        256.0f);
    m_ShaderManager->UpdateMatrixBuffer(ShaderKeys::Cloud,
        context,
        m_CloudArea->GetModelMatrix(),
        m_Camera->GetViewMatrix(),
        m_Camera->GetProjectionMatrix());
    m_ShaderManager->UpdateLightBuffer(ShaderKeys::Cloud,
        context, m_Light.get());
    m_ShaderManager->SetShaders(ShaderKeys::Cloud, context);
    m_ShaderManager->SetConstantBuffers(ShaderKeys::Cloud, context);
    m_CloudArea->Render(context);

    m_Renderer->SetAlphaBlending(false);

    m_UI->Render();
    m_Renderer->EndScene();
} // Render


void MainEngine::UpdateUI()
{
    if (m_InputManager->IsF1Toggled())
    {
        m_UI->ToggleWidget();
    }
    if (m_InputManager->IsMouseLPressed() && m_UI->CanControlWorld())
    {
        m_UI->SetCameraLocked(false);
    }
    else if (m_InputManager->IsMouseLPressed() && !m_UI->CanControlWorld())
    {
        m_UI->SetCameraLocked(true);
    }

} // UpdateUI


void MainEngine::UpdateRenderStates()
{
    m_Renderer->SetMode(m_isWireframe, m_backCullEnable);
    m_Renderer->SetDepthBuffer(m_depthEnable);
} // UpdateRenderStates