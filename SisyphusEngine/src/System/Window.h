#pragma once
#include <windows.h>
#include <string>


class Window {
public:
    Window();
    Window(const Window& other) = delete;
    ~Window();

    bool Init(WNDPROC wndProc, LPCWSTR appName);
    void Shutdown();

public:
    HWND GetHwnd() const { return m_hwnd; }
    HINSTANCE GetHinstance() const { return m_hinstance; }

private:
    HWND m_hwnd;
    HINSTANCE m_hinstance;
    LPCWSTR m_engineName;
}; // Window