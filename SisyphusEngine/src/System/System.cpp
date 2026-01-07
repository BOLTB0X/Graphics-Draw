// System/System.cpp
#include "System.h"

#include "Common/EngineSettings.h"
#include "Common/EngineHelper.h"

#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"

/* default */
/////////////////////////////////////////////////////////////////////

System::System()
	: m_applicationName(0),
	m_hinstance(0),
	m_hwnd(0),
	m_InputPtr(nullptr),
	m_Application(nullptr)
{ } // System


System::~System() { } // ~Syste


bool System::Init()
{
	bool result;
	int screenWidth = 0, screenHeight = 0;

	if (InitWindows(screenWidth, screenHeight) == false) {
		return false;
	}

	m_Application = new Application;
	result = m_Application->Init(m_hwnd, screenWidth, screenHeight);
	if (result == false) return false;

	m_InputPtr = m_InputPtr = m_Application->GetInput();
	return true;
} // Init


void System::Shutdown() {
	if (m_Application)
	{
		m_Application->Shutdown();
		delete m_Application;
		m_Application = 0;
	}

	ShutdownWindows();
} // Shutdown


void System::Run()
{
	MSG msg;
	bool done, result;
	
	ZeroMemory(&msg, sizeof(MSG));

	done = false;
	while (done == false)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (msg.message == WM_QUIT)
			done = true;
		else
		{
			result = Frame();
			if (!result)
				done = true;
		}
	}

	return;
} // Run


bool System::Frame()
{
	bool result;
	result = m_InputPtr->Frame();
	if (result == false) return false;

	result = m_Application->Frame();
	if (result == false) return false;
	return true;
} // Frame


LRESULT CALLBACK System::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	return DefWindowProc(hwnd, umsg, wparam, lparam);
} // MessageHandler
/////////////////////////////////////////////////////////////////////


/* private */
/////////////////////////////////////////////////////////////////////

bool System::InitWindows(int& screenWidth, int& screenHeight)
{
	WNDCLASSEXW wc;
	DEVMODE dmScreenSettings;
	int posX, posY;

	m_applicationName = L"Sisyphus Engine";
	ApplicationHandle = this;
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
	RegisterClassExW(&wc);

	//  GetSystemMetrics 함수는 지정된 시스템 매개변수 또는 시스템 구성 요소의 크기 및 위치를 반환
	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	// 셋업
	if (EngineSettings::FULL_SCREEN)
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

		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	m_hwnd = CreateWindowExW(WS_EX_APPWINDOW, m_applicationName, m_applicationName,
		WS_OVERLAPPEDWINDOW,
		posX, posY, screenWidth, screenHeight, NULL, NULL, m_hinstance, NULL);

	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);

	ShowCursor(true);

	return true;
} // InitWindows


void System::ShutdownWindows()
{
	ShowCursor(true);

	if (EngineSettings::FULL_SCREEN)
		ChangeDisplaySettings(NULL, 0);

	DestroyWindow(m_hwnd);
	m_hwnd = NULL;

	UnregisterClassW(m_applicationName, m_hinstance);
	m_hinstance = NULL;

	ApplicationHandle = NULL;

	return;
} // ShutdownWindows
/////////////////////////////////////////////////////////////////////

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	if (ImGui_ImplWin32_WndProcHandler(hwnd, umessage, wparam, lparam))
		return true;

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