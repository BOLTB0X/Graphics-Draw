////////////////////////////////////////////////////////////////////////////////
// Filename: FrameworkSystem.cpp
////////////////////////////////////////////////////////////////////////////////
#include "FrameworkSystem.h"


FrameworkSystem::FrameworkSystem()
{
	m_applicationName = L"Sisyphus Engine";
	m_hinstance = NULL;
	m_hwnd = NULL;

	m_Input = 0;
	m_App = 0;
} // FrameworkSystem


FrameworkSystem::FrameworkSystem(const FrameworkSystem& other)
{
	m_applicationName = L"Sisyphus Engine";
	m_hinstance = NULL;
	m_hwnd = NULL;

	m_Input = 0;
	m_App = 0;
} // FrameworkSystem


FrameworkSystem::~FrameworkSystem()
{
} // ~FrameworkSystem


bool FrameworkSystem::Init()
{
	int screenWidth, screenHeight;
	bool result;


	screenWidth = 0;
	screenHeight = 0;

	// windows api 초기화
	InitWindows(screenWidth, screenHeight);

	// user keyboard input 처리를 위한 input 객체 생성 및 초기화
	m_Input = new Input;

	m_Input->Init();

	// App 객체 생성 및 초기화
	// 이 객체 는 이 애플리케이션의 모든 그래픽 렌더링을 처리
	m_App = new App;

	result = m_App->Init(screenWidth, screenHeight, m_hwnd);
	if (!result)
	{
		return false;
	}

	return true;
} // Init


void FrameworkSystem::Shutdown()
{
	// App 객체 종료
	if (m_App)
	{
		m_App->Shutdown();
		delete m_App;
		m_App = 0;
	}

	// Input 객체 릴리즈.
	if (m_Input)
	{
		delete m_Input;
		m_Input = 0;
	}

	ShutdownWindows();

	return;
} // Shutdown


void FrameworkSystem::Run()
{
	MSG msg;
	bool done, result;


	// message 구조체 init
	ZeroMemory(&msg, sizeof(MSG));


	done = false;
	while (!done)
	{

		// Windows 메시지 큐에 메시지가 있으면 처리
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (msg.message == WM_QUIT)
		{
			done = true;
		}
		else
		{
			result = Frame();
			if (!result)
			{
				done = true;
			}
		}

	}

	return;
} // Run


bool FrameworkSystem::Frame()
{
	bool result;


	// ESC 키가 눌렸는지 확인
	if (m_Input->IsKeyDown(VK_ESCAPE))
	{
		return false;
	}

	// 여기서 그래픽 렌더링이 수행
	result = m_App->Frame();
	if (!result)
	{
		return false;
	}

	return true;
} // Frame


LRESULT CALLBACK FrameworkSystem::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	switch (umsg)
	{
		// 키보드가 눌렸는지 확인
	case WM_KEYDOWN:
	{
		// 마지막 매개변슈는 키보드 키
		m_Input->KeyDown((unsigned int)wparam);
		return 0;
	}

	// 키보드 키가 떼어졌는지 확인
	case WM_KEYUP:
	{
		// 마지막 매개변슈는 키보드 키
		m_Input->KeyUp((unsigned int)wparam);
		return 0;
	}

	default:
	{
		return DefWindowProc(hwnd, umsg, wparam, lparam);
	}
	}
} // MessageHandler


void FrameworkSystem::InitWindows(int& screenWidth, int& screenHeight)
{
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
	int posX, posY;

	// 이 객체에게 외부 포인터 GET
	ApplicationHandle = this;

	// GetModuleHandle 함수는 현재 프로세스의 인스턴스 핸들을 반환
	m_hinstance = GetModuleHandle(NULL);


	// 셋팅
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hinstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = m_applicationName;
	wc.cbSize = sizeof(WNDCLASSEX);

	// Register window class.
	RegisterClassEx(&wc);

	//  GetSystemMetrics 함수는 지정된 시스템 매개변수 또는 시스템 구성 요소의 크기 및 위치를 반환
	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);


	// 셋업
	if (FULL_SCREEN)
	{
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		posX = posY = 0;
	}
	else
	{
		screenWidth = 800;
		screenHeight = 600;

		// 화면 중앙을 어디다 둘지
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName,
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
		posX, posY, screenWidth, screenHeight, NULL, NULL, m_hinstance, NULL);

	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);

	// 커서 가리기
	ShowCursor(false);

	return;
} // InitWindows


void FrameworkSystem::ShutdownWindows()
{
	ShowCursor(true);

	if (FULL_SCREEN)
	{
		ChangeDisplaySettings(NULL, 0);
	}

	DestroyWindow(m_hwnd);
	m_hwnd = NULL;

	UnregisterClass(m_applicationName, m_hinstance);
	m_hinstance = NULL;

	ApplicationHandle = NULL;

	return;
} // ShutdownWindows


LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch (umessage)
	{
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}

	case WM_CLOSE:
	{
		PostQuitMessage(0);
		return 0;
	}

	default:
	{
		return ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
	}
	}
} // WndProc