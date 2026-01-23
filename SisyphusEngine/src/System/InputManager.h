#pragma once
#include<memory>

class Position;
class Camera;
class Input;


class InputManager {
public:
    InputManager();
    InputManager(const InputManager& other) = delete;
    ~InputManager();

    bool Init(std::shared_ptr<Input> input);
    bool Frame(float, Camera*, bool);

public:
    bool IsMouseLPressed();
    bool IsF1Toggled() const;

private:
    void UpdateMouseDelta();
    void HandleRotation(Position*);
    void HandleMovement(float, Camera*);

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
    std::shared_ptr<Input> m_Input; 
    float m_deltaX, m_deltaY;
    const float m_sensitivity;
}; // InputManager