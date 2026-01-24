// System/System.h
#pragma once
#define WIN32_LEAN_AND_MEAN
#include <memory>
#include <windows.h>


class InputManager;
class MainEngine;
class Window;


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
    std::unique_ptr<Window> m_Window;
    std::shared_ptr<InputManager> m_InputManager;
    std::unique_ptr<MainEngine> m_Engine;
}; // System


static System* ApplicationHandle = nullptr;