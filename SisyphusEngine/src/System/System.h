// System/System.h
#pragma once
#define WIN32_LEAN_AND_MEAN

#include <memory>
#include <windows.h>

class Input;
class Gui;
class MainEngine;


class System {
public:
    System();
    System(const System&) = delete;
    ~System();

    bool Init();
    void Shutdown();
    void Run();
    bool Frame();
    LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

private:
    bool InitWindows(int&, int&);
    void ShutdownWindows();

private:
    LPCWSTR m_applicationName;
    HINSTANCE m_hinstance;
    HWND m_hwnd;

private:
    std::shared_ptr<Input> m_Input;
    std::shared_ptr<Gui> m_Gui;
    std::unique_ptr<MainEngine> m_Engine;
}; // System

static System* ApplicationHandle = nullptr;