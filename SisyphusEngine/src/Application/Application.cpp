// Application/Application.h
#include "Application.h"

#include "Common/EngineHelper.h"
#include "Common/EngineSettings.h"
#include "Graphics/Shader/StoneShader.h"

#include "Framework/Widget/StatsWidget.h"
#include "Framework/Widget/InspectorWidget.h"
#include "Framework/Widget/MainSideBarWidget.h"
#include "Framework/Widget/ControlWidget.h"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

/* default */
/////////////////////////////////////////////////////////////////////

Application::Application()
	: m_Input(nullptr),
	m_Timer(nullptr),
	m_Fps(nullptr),
	m_Renderer(nullptr),
	m_Gui(nullptr),
	m_mainSideBarWidget(nullptr)
{
} // Application

Application::~Application() { } // ~Application


bool Application::Init(HWND hwnd, int screenWidth, int screenHeight)
{	
	m_Input = new Input;
	if (m_Input->Init(GetModuleHandle(NULL), hwnd, screenWidth, screenHeight) 
		== false)
	{
		EngineHelper::ErrorBox(hwnd, L"Input 객체 초기화 실패");
		return false;
	}

	m_Timer = new Timer;	
	if (m_Timer->Init() == false)
	{
		EngineHelper::ErrorBox(hwnd, L"Timer 객체 초기화 실패");
		return false;
	}

	m_Fps = new Fps;
	m_Fps->Init();

	m_Renderer = std::make_unique<Renderer>();
	if (m_Renderer->Init(hwnd, screenWidth, screenHeight)
		== false)
	{
		EngineHelper::ErrorBox(hwnd, L"Renderer 초기화 단계에서 실패했습니다.");
		return false;
	}

	m_TextureManager = std::make_unique<TexturesManager>();
	m_ShaderManager = std::make_unique<ShaderManager>();
	if (m_ShaderManager->Init(
		m_Renderer->GetDX11Device()->GetDevice(),
		hwnd) == false)
	{
		EngineHelper::ErrorBox(hwnd, L"ShaderManager 초기화 실패");
		return false;
	}

	m_ModelManager = std::make_unique<ModelManager>();

	m_World = std::make_unique<World>();
	if (m_World->Init(
		m_Renderer->GetDX11Device()->GetDevice(),
		m_Renderer->GetDeviceContext(),
		m_ModelManager.get(),
		m_TextureManager.get(),
		screenWidth, screenHeight) == false)
	{
		EngineHelper::ErrorBox(hwnd, L"World 초기화 실패");
		return false;
	}

	m_Gui = new Gui();
	if (m_Gui->Init(hwnd, 
		m_Renderer->GetDX11Device()->GetDevice(),
		m_Renderer->GetDeviceContext()) 
		== false)
	{
		EngineHelper::ErrorBox(hwnd, L"Gui 초기화 실패");
		return false;
	}

	InitGui();
	return true;
} // Init


void Application::Shutdown()
{
	if (m_Gui)
	{
		m_Gui->Shutdown();
		delete m_Gui;
		m_Gui = 0;
	}

	if (m_ModelManager)
		m_ModelManager->Shutdown();

	if (m_ShaderManager)
		m_ShaderManager->Shutdown();

	if (m_TextureManager)
		m_TextureManager->Shutdown();

	if (m_Renderer)
	{
		m_Renderer->Shutdown();
		m_Renderer = 0;
	}

	if (m_Fps)
	{
		delete m_Fps;
		m_Fps = 0;
	}

	if (m_Timer)
	{
		delete m_Timer;
		m_Timer = 0;
	}

	if (m_Input)
	{
		m_Input->Shutdown();
		delete m_Input;
		m_Input = 0;
	}

	return;
} // Shutdown


bool Application::Frame()
{
	bool result;

	if (m_Input == false) return false;
	result = m_Input->Frame();
	if (result == false) return false;
	if (m_Input->IsEscapePressed()) return false;

	m_Timer->Frame();
	m_Fps->Frame();

	HandleEditorMode();
	ImGuiIO& io = ImGui::GetIO();
	bool canControlWorld = !io.WantCaptureMouse;

	m_World->Frame(m_Timer->GetTime(), canControlWorld);

	result = Render();
	if (result == false) return false;
	
	return true;
} // Frame

/////////////////////////////////////////////////////////////////////

/* private */
/////////////////////////////////////////////////////////////////////

bool Application::Render()
{
	m_Renderer->BeginScene(0.5f, 0.0f, 0.0f, 1.0f);
	m_Gui->Begin();

	RenderGui();

	auto stoneShader = m_ShaderManager->GetShader<StoneShader>("Stone");
	if (stoneShader)
	{
		m_World->Render(m_Renderer->GetDeviceContext(), stoneShader);
	}

	m_Gui->FrameWidgets();
	m_Gui->End();
	m_Renderer->EndScene();

	return true;
} // Render


void Application::InitGui()
{
	auto sideBar = std::make_unique<MainSideBarWidget>("F1:Sisyphus Editor");
	m_mainSideBarWidget = sideBar.get();

	sideBar->AddComponent(std::make_unique<StatsWidget>(m_Fps, m_Timer));
	sideBar->AddComponent(std::make_unique<ControlWidget>(m_Renderer.get()));

	auto inspector = std::make_unique<InspectorWidget>();
	inspector->SetActorList(m_World->GetActors());
	if (m_World->GetActors().empty() == false)
		inspector->SetTarget(m_World->GetActors()[0].get());

	sideBar->AddComponent(std::move(inspector));

	m_Gui->AddWidget(std::move(sideBar));
	return;
} // InitGui


void Application::RenderGui()
{
	if (m_mainSideBarWidget && m_mainSideBarWidget->IsVisible())
	{
		auto control = m_mainSideBarWidget->GetComponent<ControlWidget>();
		if (control)
		{
			m_Renderer->GetRasterizer()->Bind(
				m_Renderer->GetDeviceContext(),
				control->IsWireframe(),
				control->IsCullNone()
			);
		}
	}

	return;
} // RenderGui


void Application::HandleEditorMode()
{
	if (m_Input == nullptr || m_mainSideBarWidget == nullptr)
		return;

	if (m_Input->IsF1Toggled())
	{
		m_mainSideBarWidget->SetVisible(m_mainSideBarWidget->IsVisible() == false);
	}
	return;
} // HandleEditorMode