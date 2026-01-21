#include "MainEngine.h"
#include "Timer.h"
#include "Fps.h"
#include "Camera.h"
// System
#include "Input.h"
#include "Gui.h"
// UI
#include "UI.h"
#include "UserInput.h"
// Common
#include "EngineSettings.h"
#include "EngineHelper.h"
// Framework
#include "Renderer.h"
//
#include "TexturesManager.h"
//
#include "CloudShader.h"
#include "GeometryArea.h"


MainEngine::MainEngine() : m_Renderer(nullptr) {}

MainEngine::~MainEngine() {}


bool MainEngine::Init(HWND hwnd, std::shared_ptr<Input> input, std::shared_ptr<Gui> gui)
{
    m_Input = input;
    m_Gui = gui;

    m_Timer = std::make_unique<Timer>();
    if (m_Timer->Init() == false) return false;
    m_Fps = std::make_unique<Fps>();
    m_Fps->Init();

    m_Renderer = std::make_unique<Renderer>();
    if (m_Renderer->Init(hwnd, true) == false) return false;

    m_Camera = std::make_unique<Camera>();
    m_Camera->SetPosition(0.0f, 0.0f, -5.0f);
    m_Camera->InitView();
    m_Camera->InitProjection(
        EngineSettings::SCREEN_WIDTH, EngineSettings::SCREEN_HEIGHT,
        EngineSettings::SCREEN_NEAR, EngineSettings::SCREEN_DEPTH
    );

    m_TexturesManager = std::make_unique<TexturesManager>();
    if (m_TexturesManager->Init(
        m_Renderer->GetDevice(),
        m_Renderer->GetDeviceContext())
    == false) return false; 

    m_CloudShader = std::make_unique<CloudShader>();
    if (m_CloudShader->Init(m_Renderer->GetDevice(), 
        hwnd, L"HLSL/CloudVS.hlsl", 
        L"HLSL/CloudPS.hlsl") == false)
    {
        return false;
    }

    m_CloudArea = std::make_unique<GeometryArea>();
    if (m_CloudArea->Init(m_Renderer->GetDevice()) == false)
    {
        return false;
    }
    
    if (gui->Init(hwnd,
        m_Renderer->GetDevice(),
        m_Renderer->GetDeviceContext())
        == false) return false;
    m_UserInput = std::make_unique<UserInput>();
    if (m_UserInput->Init(input) == false) return false;
    m_UI = std::make_unique<UI>();
    if (m_UI->Init(gui) == false) return false;
    
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

    if (m_UserInput->IsEscapePressed()) return false;

    // 렌더링 로직
    Render();

    return true;
} // Frame


void MainEngine::Render()
{
    m_Renderer->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

    ID3D11DeviceContext* context = m_Renderer->GetDeviceContext();

    m_TexturesManager->PSSetShaderResources(context, EngineSettings::NOISE_PATH, 0);
   
    m_CloudShader->UpdateMatrixBuffer(
        context,
        m_CloudArea->GetModelMatrix(),
        m_Camera->GetViewMatrix(),
        m_Camera->GetProjectionMatrix());

    m_CloudShader->UpdateGlobalBuffer(
        context,
        m_Timer->GetTime(),
        256.0f
    );

    m_CloudShader->BindShader(context);
    m_CloudShader->BindConstantBuffers(context);

    m_CloudArea->Render(context);
    context->DrawIndexed(m_CloudArea->GetIndexCount(), 0, 0);

    m_Renderer->EndScene();
} // Render