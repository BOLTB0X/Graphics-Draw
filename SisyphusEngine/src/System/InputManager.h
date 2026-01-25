#pragma once
#include<memory>
#include "Input.h"
// Common
#include "PropertyHelper.h"

class Position;
class Camera;

class InputManager {
public:
    InputManager();
    InputManager(const InputManager& other) = delete;
    ~InputManager();

    bool Init(HINSTANCE, HWND);
    bool Init(std::shared_ptr<Input> input);
    void Shutdown();
    
    bool Frame(); // 로우 레벨 입력 및 PeekMessage 상태 갱신
    bool Frame(float, Position*, PropertyHelper::Property<float>, bool); // 카메라 및 게임 로직 업데이트

public:
    bool IsMouseLPressed();
    bool IsF1Toggled() const;

private:
    void UpdateMouseDelta();
    void HandleRotation(Position*);
    void HandleMovement(float, Position*, PropertyHelper::Property<float>);

    void GetMouseDelta(float&, float&);
    float GetZoom() const;

private:
    bool IsEscapePressed() const;
    bool IsWPressed() const;
    bool IsSPressed() const;
    bool IsAPressed() const;
    bool IsDPressed() const;
    bool IsZPressed() const;
    bool IsXPressed() const;

private:
    std::unique_ptr<Input> m_Input;
    float m_deltaX, m_deltaY;
    const float m_sensitivity;
}; // InputManager