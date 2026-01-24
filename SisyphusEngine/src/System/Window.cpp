#include "Window.h"
// Common
#include "EngineSettings.h"

Window::Window()
    : m_hwnd(nullptr),
    m_hinstance(nullptr),
    m_engineName(nullptr)
{
} // Window


Window::~Window()
{ 
    Shutdown(); 
} // ~Window


bool Window::Init(WNDPROC wndProc, LPCWSTR appName)
{
    m_engineName = appName;
    m_hinstance = GetModuleHandle(NULL);

    WNDCLASSEXW wc;
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.lpfnWndProc = wndProc; // System에서 넘겨준 WndProc 연결
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = m_hinstance;
    wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
    wc.hIconSm = wc.hIcon;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = m_engineName;
    wc.cbSize = sizeof(WNDCLASSEX);

    if (!RegisterClassExW(&wc)) return false;

    int posX, posY;
    if (EngineSettings::FULL_SCREEN)
    {
        posX = posY = 0;
    }
    else
    {
        RECT windowRect = { 0, 0, (LONG)EngineSettings::SCREEN_WIDTH, (LONG)EngineSettings::SCREEN_HEIGHT };
        AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);
        int actualWidth = windowRect.right - windowRect.left;
        int actualHeight = windowRect.bottom - windowRect.top;

        posX = (GetSystemMetrics(SM_CXSCREEN) - actualWidth) / 2;
        posY = (GetSystemMetrics(SM_CYSCREEN) - actualHeight) / 2;
    }

    m_hwnd = CreateWindowExW(WS_EX_APPWINDOW, m_engineName, m_engineName,
        WS_OVERLAPPEDWINDOW, posX, posY, EngineSettings::SCREEN_WIDTH, EngineSettings::SCREEN_HEIGHT,
        NULL, NULL, m_hinstance, NULL);

    if (!m_hwnd)
        return false;

    RECT clientRect;
    GetClientRect(m_hwnd, &clientRect);
    EngineSettings::SCREEN_WIDTH = clientRect.right - clientRect.left;
    EngineSettings::SCREEN_HEIGHT = clientRect.bottom - clientRect.top;

    ShowWindow(m_hwnd, SW_SHOW);
    SetForegroundWindow(m_hwnd);
    SetFocus(m_hwnd);
    ShowCursor(true);

    return true;
} // Init


void Window::Shutdown()
{
    ShowCursor(true);

    if (EngineSettings::FULL_SCREEN)
    {
        ChangeDisplaySettings(NULL, 0);
    }

    if (m_hwnd)
    {
        DestroyWindow(m_hwnd);
        m_hwnd = nullptr;
    }

    if (m_engineName)
    {
        UnregisterClassW(m_engineName, m_hinstance);
        m_engineName = nullptr;
    }

    m_hinstance = nullptr;
} // Shutdown