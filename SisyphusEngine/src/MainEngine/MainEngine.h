#pragma once
#include <memory>
#include <windows.h>

class Input;
class Timer;
class Camera;
class Renderer;
class Fps;
class UI;
class InputManager;
class TexturesManager;
class DefaultModel;
class Light;
class ShaderManager;

class MainEngine {
public:
    MainEngine();
    MainEngine(const MainEngine& other) = delete;
    ~MainEngine();

    bool Init(HWND hwnd, std::shared_ptr<InputManager>);
    void Shutdown();
    bool Frame(); // 프레임 루프

private:
    void Render();
    void UpdateUI();
    void UpdateRenderStates();

private:
    // 공유
    std::shared_ptr<InputManager> m_InputManager;
    // 유틸
    std::unique_ptr<Timer> m_Timer;
    std::unique_ptr<Fps> m_Fps;
    // 인터페이스
    //std::unique_ptr<InputManager> m_InputManager;
    std::unique_ptr<UI> m_UI;
    // 그래픽
    std::unique_ptr<Renderer> m_Renderer;
    std::unique_ptr<Camera> m_Camera;
    // 매니저
    std::unique_ptr<TexturesManager> m_TexturesManager;
    std::unique_ptr<ShaderManager> m_ShaderManager;
    // TODO: RenderingEngie 으로 옮길 예정
    std::unique_ptr<DefaultModel> m_CloudArea; // 레이마칭용 Quad
    std::unique_ptr<Light> m_Light; // 광원 데이터 (위치, 색상, 강도)
    std::unique_ptr<DefaultModel> m_SunModel;

    bool m_isWireframe;
    bool m_backCullEnable;
    bool m_depthEnable;
}; // MainEngine