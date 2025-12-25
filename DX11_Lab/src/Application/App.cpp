////////////////////////////////////////////////////////////////////////////////
// Filename: App.cpp
////////////////////////////////////////////////////////////////////////////////
#include "Application/App.h"


App::App()
{
	m_Direct3D = 0;
	m_Camera = 0;
	m_Model = 0;
	m_TextureShader = 0;
} // App


App::App(const App& other)
{
	m_Direct3D = 0;
	m_Camera = 0;
	m_Model = 0;
	m_TextureShader = 0;
} // App


App::~App()
{
} // ~App


bool App::Init(int screenWidth, int screenHeight, HWND hwnd)
{
	char textureFilename[128];
	bool result;

	m_Direct3D = new D3DRenderer;
	result = m_Direct3D->Init(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBoxW(hwnd, L"Direct3D 초기화 실패", L"Error", MB_OK);
		return false;
	}

	m_Camera = new Camera;
	m_Camera->SetPosition(0.0f, 0.0f, -5.0f);

	m_Model = new Model;
	strcpy_s(textureFilename, "./assets/stone01.tga");

	result = m_Model->Init(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), textureFilename);
	if (!result)
	{
		MessageBoxW(hwnd, L"Model 클래스 초기화 실패.", L"Error", MB_OK);
		return false;
	}

	m_TextureShader = new Shader;

	result = m_TextureShader->Init(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBoxW(hwnd, L"Shader 클래스 초기화 실패", L"Error", MB_OK);
		return false;
	}

	return true;
} // Init


void App::Shutdown()
{
	if (m_TextureShader)
	{
		m_TextureShader->Shutdown();
		delete m_TextureShader;
		m_TextureShader = 0;
	}

	if (m_Model)
	{
		m_Model->Shutdown();
		delete m_Model;
		m_Model = 0;
	}

	if (m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}

	if (m_Direct3D)
	{
		m_Direct3D->Shutdown();
		delete m_Direct3D;
		m_Direct3D = 0;
	}
	return;
} // Shutdown


bool App::Frame()
{
	bool result;


	// Render the graphics scene.
	result = Render();
	if (!result)
	{
		return false;
	}

	return true;
} // Frame


bool App::Render()
{
	//m_Direct3D->BeginScene(0.5f, 0.5f, 0.5f, 1.0f);
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	bool result;

	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);
	m_Camera->Render(); // 카메라 행렬 계산
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	// 모델의 버텍스 및 인덱스 버퍼를 그래픽 파이프라인에 배치하여 렌더링 준비
	m_Model->Render(m_Direct3D->GetDeviceContext());
	result = m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model->GetTexture());
	if (!result)
	{
		return false;
	}

	m_Direct3D->EndScene();
	return true;
} // Render