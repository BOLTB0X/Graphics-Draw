////////////////////////////////////////////////////////////////////////////////
// Filename: Application.cpp
////////////////////////////////////////////////////////////////////////////////
#include "Application.h"


Application::Application()
	: m_Direct3D(nullptr),
	m_Model(nullptr),
	m_TextureManager(nullptr),
	m_ShaderManager(nullptr),
	m_Input(nullptr),
	m_Timer(nullptr),
	m_Fps(nullptr),
	m_Zone(nullptr)
{
} // Application


Application::Application(const Application& other)
	: m_Direct3D(nullptr),
	m_Model(nullptr),
	m_TextureManager(nullptr),
	m_ShaderManager(nullptr),
	m_Input(nullptr),
	m_Timer(nullptr),
	m_Fps(nullptr),
	m_Zone(nullptr)
{
} // Application


Application::~Application()
{
} // ~Application


bool Application::Init(int screenWidth, int screenHeight, HWND hwnd)
{
	char modelFilename[128], textureFilename1[128], textureFilename2[128];
	bool result;

	m_Input = new Input;
	if (!m_Input)
	{
		return false;
	}
	result = m_Input->Init(GetModuleHandle(NULL), hwnd, screenWidth, screenHeight);
	if (!result)
	{
		MessageBoxW(hwnd, L"Input 객체 초기화 실패", L"Error", MB_OK);
		return false;
	}

	m_Direct3D = new D3DRenderer;
	result = m_Direct3D->Init(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBoxW(hwnd, L"Direct3D 초기화 실패", L"Error", MB_OK);
		return false;
	}

	strcpy_s(modelFilename, "assets/Stone/sphere.txt");
	strcpy_s(textureFilename1, "assets/Stone/stone01.tga");
	strcpy_s(textureFilename2, "assets/Stone/normal01.tga");

	m_Model = new Model;

	result = m_Model->Init(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(),
		modelFilename,
		textureFilename1,
		textureFilename2);
	if (!result)
	{
		MessageBoxW(hwnd, L"Model 클래스 초기화 실패.", L"Error", MB_OK);
		return false;
	}

	m_ShaderManager = new ShaderManager;
	if (!m_ShaderManager)
	{
		return false;
	}
	result = m_ShaderManager->Init(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBoxW(hwnd, L"ShaderManager 클래스 초기화 실패", L"Error", MB_OK);
		return false;
	}

	m_TextureManager = new TextureManager;
	if (!m_TextureManager)
	{
		return false;
	}

	result = m_TextureManager->Init(10);
	if (!result)
	{
		MessageBoxW(hwnd, L"TextureManager 클래스 초기화 실패.", L"Error", MB_OK);
		return false;
	}

	result = m_TextureManager->LoadTexture(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), "assets/Terrain/textures/dirt01d.tga", 0);
	if (!result)
	{
		return false;
	}

	result = m_TextureManager->LoadTexture(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), "assets/Terrain/textures/dirt01n.tga", 1);
	if (!result)
	{
		return false;
	}


	m_Timer = new Timer;
	if (!m_Timer)
	{
		return false;
	}
	result = m_Timer->Init();
	if (!result)
	{
		MessageBoxW(hwnd, L"Timer 객체 초기화 실패.", L"Error", MB_OK);
		return false;
	}

	m_Fps = new Fps;
	if (!m_Fps)
	{
		return false;
	}
	m_Fps->Init();

	m_Zone = new Zone;
	if (!m_Zone)
	{
		return false;
	}
	result = m_Zone->Init(m_Direct3D, hwnd, screenWidth, screenHeight, SCREEN_DEPTH, m_Model);
	if (!result)
	{
		MessageBoxW(hwnd, L"Zone 객체 초기화 실패", L"Error", MB_OK);
		return false;
	}

	return true;
} // Init


void Application::Shutdown()
{
	if (m_Zone)
	{
		m_Zone->Shutdown();
		delete m_Zone;
		m_Zone = 0;
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

	if (m_TextureManager)
	{
		m_TextureManager->Shutdown();
		delete m_TextureManager;
		m_TextureManager = 0;
	}

	if (m_ShaderManager)
	{
		m_ShaderManager->Shutdown();
		delete m_ShaderManager;
		m_ShaderManager = 0;
	}

	if (m_Model)
	{
		m_Model->Shutdown();
		delete m_Model;
		m_Model = 0;
	}

	if (m_Direct3D)
	{
		m_Direct3D->Shutdown();
		delete m_Direct3D;
		m_Direct3D = 0;
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

	if (!m_Input)
		return false;

	result = m_Input->Frame();
	if (!result)
		return false;

	if (m_Input->IsEscapePressed())
		return false;

	m_Timer->Frame();
	m_Fps->Frame();

	result = m_Zone->Frame(m_Direct3D, m_Input, m_ShaderManager, m_TextureManager, m_Timer->GetTime(), m_Fps->GetFps());
	if (!result)
	{
		return false;
	}

	result = Render();
	if (!result)
	{
		return false;
	}

	return true;
} // Frame


Input* Application::GetInput() {
	return m_Input;
} // GetInput


bool Application::Render()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	bool result;

	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	result = m_Zone->Render(m_Direct3D, m_ShaderManager, m_TextureManager);
	if (!result)
		return false;

	m_Direct3D->EndScene();

	return true;
} // Render