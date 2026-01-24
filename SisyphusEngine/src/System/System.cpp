// System/System.cpp
#include "System.h"
#include "InputManager.h"
#include "Window.h"
// Common
#include "EngineSettings.h"
#include "EngineHelper.h"
// MainEngine
#include "MainEngine.h"
// imgui
#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"


System::System()
{
	m_Window = std::make_unique<Window>();
	m_InputManager = std::make_shared<InputManager>();
	m_Engine = std::make_unique<MainEngine>();
} // System


System::~System()
{
} // ~System


bool System::Init()
{
	ApplicationHandle = this;

	if (m_Window->Init(WndProc, L"Sisyphus Engine") == false)
	{
		EngineHelper::ErrorBox(m_Window->GetHwnd(), L"m_Window->Init 실패");
		return false;
	}

	if (m_InputManager->Init(
		m_Window->GetHinstance(),
		m_Window->GetHwnd())
		== false)
	{
		EngineHelper::ErrorBox(m_Window->GetHwnd(), L"m_InputManager->Init 실패");
		return false;
	}


	if (m_Engine->Init(
		m_Window->GetHwnd(), 
		m_InputManager) == false)
	{
		EngineHelper::ErrorBox(m_Window->GetHwnd(), L"m_Engine->Init 실패");
		return false;
	}


	return true;
} // Init


void System::Shutdown()
{	
	if (m_Engine)
	{
		m_Engine->Shutdown();
		m_Engine.reset();
	}

	if (m_InputManager)
		m_InputManager->Shutdown();

	if (m_Window) m_Window->Shutdown();

	ApplicationHandle = nullptr;
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
	if (m_InputManager->Frame()
		== false) return false;

	if (m_Engine->Frame()
		== false) return false;
	return true;
} // Frame


LRESULT CALLBACK System::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	return DefWindowProc(hwnd, umsg, wparam, lparam);
} // MessageHandler


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