//#include "App.h"
//#include "Window/Win32Window.h"
//#include "Renderer/Core/DX11Renderer.h"
//
//App::App(HINSTANCE hInstance)
//    : m_hInstance(hInstance),
//    m_RockModel(nullptr),
//    m_TextureShader(nullptr),
//    m_World(DirectX::XMMatrixIdentity()),
//    m_View(DirectX::XMMatrixIdentity()),
//    m_Proj(DirectX::XMMatrixIdentity())
//{
//    m_RockModel = new Model();
//    m_TextureShader = new TextureShader();
//}
//
//App::~App() {
//    if (m_TextureShader) {
//        delete m_TextureShader;
//        m_TextureShader = nullptr;
//    }
//    if (m_RockModel) {
//        delete m_RockModel;
//        m_RockModel = nullptr;
//    }
//} // ~App
//
//void App::Run()
//{
//    Win32Window window(m_hInstance, 1280, 720);
//
//    DX11Renderer renderer;
//    if (!renderer.Init(window.GetHWND()))
//        return;
//
//    // 1번 체크: Init 자체가 실패하는가?
//    if (!renderer.Init(window.GetHWND())) {
//        MessageBoxW(NULL, L"Renderer Init 자체가 실패함!", L"Error", MB_OK);
//        return;
//    }
//
//    if (renderer.GetDevice() == nullptr) {
//        MessageBoxW(NULL, L"GetDevice()가 NULL을 반환", L"Error", MB_OK);
//        return;
//    }
//
//
//    //MessageBoxW(NULL, L"여기까지 오면 Renderer는 정상임!", L"Success", MB_OK);
//
//    if (!m_RockModel->Load(renderer.GetDevice(), "assets/rock_base/rock_base_lp.fbx")) {
//        MessageBoxW(window.GetHWND(), L"Failed to load Model", L"Error", MB_OK);
//    }
//
//    m_TextureShader->Init(
//        renderer.GetDevice(),
//        window.GetHWND(),
//        L"shaders/TextureVS.hlsl",
//        L"shaders/TexturePS.hlsl"
//    );
//
//    m_View = DirectX::XMMatrixLookAtLH(
//        { 0.0f, 2.0f, -5.0f }, // 카메라 위치
//        { 0.0f, 0.0f, 0.0f },  // 바라보는 지점
//        { 0.0f, 1.0f, 0.0f }   // 하늘 방향
//    );
//
//    m_Proj = DirectX::XMMatrixPerspectiveFovLH(
//        DirectX::XM_PIDIV4,    // 45도 시야각
//        1280.0f / 720.0f,      // 종횡비
//        0.1f, 1000.0f          // 근평면/원평면
//    );
//
//    m_World = DirectX::XMMatrixScaling(1000.0f, 1000.0f, 1000.0f);
//    m_World *= DirectX::XMMatrixTranslation(0.0f, 0.0f, 0.0f);
//
//    while (!window.ShouldClose())
//    {
//        window.PollEvents();
//
//        renderer.BeginFrame();
//       
//        // Draw
//        m_RockModel->Draw(renderer.GetContext(), m_TextureShader, m_World, m_View, m_Proj);
//
//        renderer.EndFrame();
//    }
//
//    renderer.Shutdown();
//} // Run

////////////////////////////////////////////////////////////////////////////////
// Filename: App.cpp
////////////////////////////////////////////////////////////////////////////////
#include "Application/App.h"


App::App()
{
	m_Direct3D = 0;
	m_Camera = 0;
	m_Model = 0;
	m_ColorShader = 0;
} // App


App::App(const App& other)
{
	m_Direct3D = 0;
	m_Camera = 0;
	m_Model = 0;
	m_ColorShader = 0;
} // App


App::~App()
{
} // ~App


bool App::Init(int screenWidth, int screenHeight, HWND hwnd)
{
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
	result = m_Model->Init(m_Direct3D->GetDevice());
	if (!result)
	{
		MessageBoxW(hwnd, L"Model 클래스 초기화 실패.", L"Error", MB_OK);
		return false;
	}

	m_ColorShader = new ColorShader;

	result = m_ColorShader->Init(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBoxW(hwnd, L"Shader 클래스 초기화 실패", L"Error", MB_OK);
		return false;
	}

	return true;
} // Init


void App::Shutdown()
{
	if (m_ColorShader)
	{
		m_ColorShader->Shutdown();
		delete m_ColorShader;
		m_ColorShader = 0;
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
	result = m_ColorShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix);
	if (!result)
	{
		return false;
	}

	m_Direct3D->EndScene();
	return true;
} // Render