#pragma once
#include <windows.h>

class Win32Window
{
public:
    Win32Window(HINSTANCE hInstance, int width, int height);
    ~Win32Window();

    void PollEvents();
    bool ShouldClose() const;
    HWND GetHWND() const;

private:
    HWND m_hWnd = nullptr;
    bool m_ShouldClose = false;

    static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
};
