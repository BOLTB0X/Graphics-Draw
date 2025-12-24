#include "Win32Window.h"

Win32Window::Win32Window(HINSTANCE hInstance, int width, int height)
{
    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"DX11_Lab_Window";

    RegisterClassExW(&wc);

    m_hWnd = CreateWindowExW(
        0,
        wc.lpszClassName,
        L"DX11 Lab",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        width, height,
        nullptr,
        nullptr,
        hInstance,
        nullptr
    );

    ShowWindow(m_hWnd, SW_SHOW);
} // Win32Window

Win32Window::~Win32Window()
{
    DestroyWindow(m_hWnd);
} // ~Win32Window

void Win32Window::PollEvents()
{
    MSG msg{};
    while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE))
    {
        if (msg.message == WM_QUIT)
            m_ShouldClose = true;

        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
} // PollEvents

bool Win32Window::ShouldClose() const
{
    return m_ShouldClose;
} // ShouldClose

HWND Win32Window::GetHWND() const
{
    return m_hWnd;
} // GetHWND

LRESULT CALLBACK Win32Window::WndProc(
    HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg == WM_DESTROY)
    {
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(hWnd, msg, wParam, lParam);
} // WndProc
