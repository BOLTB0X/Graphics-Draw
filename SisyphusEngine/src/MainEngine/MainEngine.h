#pragma once
#include <memory>
#include <windows.h>

class Input;
class Gui;
class Timer;
class Camera;
class Renderer;
class Fps;
class UI;
class UserInput;
class CloudShader;
class GeometryArea;
class TexturesManager;

class MainEngine {
public:
    MainEngine();
    MainEngine(const MainEngine& other) = delete;
    ~MainEngine();

    bool Init(HWND hwnd, std::shared_ptr<Input>, std::shared_ptr<Gui>);
    void Shutdown();
    bool Frame(); // 프레임 루프

private:
    void Update(float deltaTime);
    void Render();

private:
    // 공유
    std::shared_ptr<Input> m_Input;
    std::shared_ptr<Gui> m_Gui;
    // 유틸
    std::unique_ptr<Timer> m_Timer;
    std::unique_ptr<Fps> m_Fps;
    // 인터페이스
    std::unique_ptr<UserInput> m_UserInput;
    std::unique_ptr<UI> m_UI;
    // 그래픽
    std::unique_ptr<Renderer> m_Renderer;
    std::unique_ptr<Camera> m_Camera;
    // 매니저
    std::unique_ptr<TexturesManager> m_TexturesManager;
    // TODO: RenderingEngie 으로 옮길 예정
    std::unique_ptr<CloudShader> m_CloudShader;
    std::unique_ptr<GeometryArea> m_CloudArea;
}; // MainEngine