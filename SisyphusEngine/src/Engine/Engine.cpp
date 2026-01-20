#include "Engine.h"
#include "Timer.h"
#include "Fps.h"
// System
#include "Input.h"
#include "Gui.h"
// UI
#include "UI.h"
#include "UserInput.h"
// Common
#include "EngineSettings.h"
#include "EngineHelper.h"
// 
#include "Renderer.h" // 곧 만들 클래스
#include "Camera.h"


Engine::Engine() : m_Renderer(nullptr) {}

Engine::~Engine() {}

bool Engine::Init(HWND hwnd, std::shared_ptr<Input> input, std::shared_ptr<Gui> gui)
{
    m_Input = input;
    m_Gui = gui;

    m_Timer = std::make_unique<Timer>();
    if (m_Timer->Init() == false) return false;
    m_Fps = std::make_unique<Fps>();
    m_Fps->Init();

    m_Renderer = std::make_unique<Renderer>();
    if (m_Renderer->Init(hwnd) == false) return false;
    m_Camera = std::make_unique<Camera>();
    m_Camera->InitProjection(
        EngineSettings::SCREEN_WIDTH, EngineSettings::SCREEN_HEIGHT,
        EngineSettings::SCREEN_NEAR, EngineSettings::SCREEN_DEPTH
    );
    
    if (gui->Init(hwnd,
        m_Renderer->GetDevice(),
        m_Renderer->GetContext())
        == false) return false;
    m_UserInput = std::make_unique<UserInput>();
    if (m_UserInput->Init(input) == false) return false;
    m_UI = std::make_unique<UI>();
    if (m_UI->Init(gui) == false) return false;
    
    return true;
}

void Engine::Shutdown()
{

    if (m_Renderer) m_Renderer->Shutdown();
}

bool Engine::Frame()
{
    m_Timer->Frame();
    m_Fps->Frame();

    if (m_UserInput->IsEscapePressed()) return false;

    // 렌더링 로직
    Render();

    return true;
}

void Engine::Render()
{
    m_Renderer->BeginScene(1.0f, 0.0f, 0.0f, 1.0f);

    // TODO: 여기에 물체 그리기 로직이 들어감

    m_Renderer->EndScene();
} // Render