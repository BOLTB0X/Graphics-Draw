////////////////////////////////////////////////////////////////////////////////
// Filename: App.cpp
////////////////////////////////////////////////////////////////////////////////
#include "App.h"


App::App()
	: m_Direct3D(nullptr),
	m_Camera(nullptr),
	m_Model(nullptr),
	m_PlayerStone(nullptr),
	m_WallStone(nullptr),
	m_Light(nullptr),
	m_ShaderManager(nullptr),
	m_Input(nullptr)
{
} // App


App::App(const App& other)
	: m_Direct3D(nullptr),
	m_Camera(nullptr),
	m_Model(nullptr),
	m_PlayerStone(nullptr),
	m_WallStone(nullptr),
	m_Light(nullptr),
	m_ShaderManager(nullptr),
	m_Input(nullptr)
{
} // App


App::~App()
{
} // ~App


bool App::Init(int screenWidth, int screenHeight, HWND hwnd)
{
	char modelFilename[128], textureFilename1[128], textureFilename2[128];
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
	m_Camera->Render();

	strcpy_s(modelFilename, "./assets/Stone/sphere.txt");

	strcpy_s(textureFilename1, "./assets/Stone/stone01.tga");
	strcpy_s(textureFilename2, "./assets/Stone/normal01.tga");

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

	// 조종할 돌 (오른쪽에 배치)
	m_PlayerStone = new Stone;
	m_PlayerStone->Init(m_Model, 1.5f, 0.0f, 0.0f, 1.0f); // 위치 x=1.5, 반지름=1.0

	// 벽이 될 돌 (왼쪽에 배치)
	m_WallStone = new Stone;
	m_WallStone->Init(m_Model, -1.5f, 0.0f, 0.0f, 1.0f); // 위치 x=-1.5, 반지름=1.0

	m_Light = new Light;

	m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetDirection(0.0f, 0.0f, 1.0f);

	m_ShaderManager = new ShaderManager;

	result = m_ShaderManager->Init(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBoxW(hwnd, L"Shader 클래스 초기화 실패", L"Error", MB_OK);
		return false;
	}

	return true;
} // Init


void App::Shutdown()
{
	if (m_ShaderManager)
	{
		m_ShaderManager->Shutdown();
		delete m_ShaderManager;
		m_ShaderManager = 0;
	}

	if (m_Light)
	{
		delete m_Light;
		m_Light = 0;
	}

	if (m_WallStone)
	{
		delete m_WallStone;
		m_WallStone = 0;
	}

	if (m_PlayerStone)
	{
		delete m_PlayerStone;
		m_PlayerStone = 0;
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


bool App::Frame(Input* input)
{
	bool result;

	m_Input = input;
	if (!m_Input) return false;

	float moveSpeed = 0.01f;
	float rotateSpeed = 0.05f;

	if (input->IsEscapePressed())
	{
		return false;
	}

	if (m_Input->IsLeftArrowPressed()) {
		m_PlayerStone->Move(-moveSpeed, 0, 0);
		m_PlayerStone->Update(0.0f, -rotateSpeed);
	}
	if (m_Input->IsRightArrowPressed()) {
		m_PlayerStone->Move(moveSpeed, 0, 0);
		m_PlayerStone->Update(0.0f, rotateSpeed);
	}

	int wheelDelta = m_Input->GetMouseWheelDelta();
	if (wheelDelta != 0)
	{
		// wheelDelta는 위로 굴리면 양수(+120 등), 아래로 굴리면 음수(-120 등)
		m_Camera->Zoom((float)wheelDelta);
	}

	m_Camera->Render();

	if (m_PlayerStone->CheckCollision(m_WallStone)) {
		if (m_Input->IsLeftArrowPressed())
		{
			m_WallStone->Move(-moveSpeed * 0.5f, 0.0f, 0.0f);
			m_WallStone->Update(0.0f, -rotateSpeed * 0.5f);

			m_PlayerStone->Move(moveSpeed * 0.5f, 0.0f, 0.0f);
		}
	}

	result = Render();
	if (!result)
	{
		return false;
	}

	return true;
} // Frame


bool App::Render()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	bool result;

	// 1. Scene 시작
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// 2. Comm 행렬 설정
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);


	// 3. 모델 버퍼, 셰이더 설정 및 렌더링
	result = m_PlayerStone->Render(m_Direct3D->GetDeviceContext(), m_ShaderManager,
		viewMatrix, projectionMatrix, m_Light);
	if (!result) 
		return false;

	result = m_WallStone->Render(m_Direct3D->GetDeviceContext(), m_ShaderManager,
		viewMatrix, projectionMatrix, m_Light);
	if (!result)
		return false;

	// 4. Scene 종료
	m_Direct3D->EndScene();

	return true;
} // Render