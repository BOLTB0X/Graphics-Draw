#include "Pch.h"
#include "InputManager.h"
// Framework
#include "Position.h"
// Enbgine
#include "Camera.h"
// Common
#include "MathHelper.h"


using namespace DirectX;
using namespace PropertyHelper;


InputManager::InputManager()
	: m_deltaX(0.0f),
	m_deltaY(0.0f),
    m_sensitivity(0.05f)
{
    m_Input = std::make_unique<Input>();
} // InputManager


InputManager::~InputManager()
{
    Shutdown();
} // ~InputManager

bool InputManager::Init(HINSTANCE hinstance, HWND hwnd)
{
    return m_Input->Init(hinstance, hwnd);
} // Init


void InputManager::Shutdown()
{
    m_Input->Shutdown();
} // Shutdown


bool InputManager::Frame()
{
    return m_Input->Frame();
} // Frame


bool InputManager::Frame(float deltaTime, Position* camPos, Property<float> fovAddProp, bool isCameraLocked)
{
    if (m_Input == nullptr || camPos == nullptr) return false;

    if (IsEscapePressed()) return false;

    UpdateMouseDelta();

    if (isCameraLocked) return true;

    if (IsMouseLPressed())
        HandleRotation(camPos);

    HandleMovement(deltaTime, camPos, fovAddProp);
    return true;
} // Frame


void InputManager::UpdateMouseDelta()
{
    int rawX, rawY;
    m_Input->GetMouseDelta(rawX, rawY);

    m_deltaX = static_cast<float>(rawX) * m_sensitivity;
    m_deltaY = static_cast<float>(rawY) * m_sensitivity;
} // UpdateMouseDelta


void InputManager::HandleRotation(Position* camPos)
{
    DirectX::XMFLOAT3 rot = camPos->GetRotation();
    float newPitch = rot.x + m_deltaY;

    if (newPitch > 89.0f) newPitch = 89.0f;
    if (newPitch < -89.0f) newPitch = -89.0f;

    camPos->SetRotation(newPitch, rot.y + m_deltaX, rot.z);
} // HandleRotation


void InputManager::HandleMovement(float deltaTime, Position* camPos, Property<float> fovAddProp)
{
    camPos->SetFrameTime(deltaTime);

    // 키보드 이동 로직 호출
    camPos->MoveForward(IsWPressed());
    camPos->MoveBackward(IsSPressed());
    camPos->TurnLeft(IsAPressed());
    camPos->TurnRight(IsDPressed());
    camPos->MoveUpward(IsZPressed());
    camPos->MoveDownward(IsXPressed());

    float wheelDelta = static_cast<float>(m_Input->GetMouseWheelDelta());
    if (wheelDelta != 0.0f)
    {
        float newFov = wheelDelta * 0.05f;
        fovAddProp.Set(newFov);
    }
} // HandleMovement


bool InputManager::IsMouseLPressed()
{ 
    return m_Input->IsMouseLPressed();
} // IsMouseLPressed


void InputManager::GetMouseDelta(float& dx, float& dy)
{ 
    dx = m_deltaX;
    dy = m_deltaY;
} // GetMouseDelta


float InputManager::GetZoom() const
{
    return static_cast<float>(m_Input->GetMouseWheelDelta());
} // GetZoom


bool InputManager::IsEscapePressed() const
{
    return m_Input->IsEscapePressed();
} // IsEscapePressed


bool InputManager::IsF1Toggled() const
{
    return m_Input->IsF1Toggled();
} // IsF1Toggled


bool InputManager::IsWPressed() const
{ 
    return m_Input->IsWPressed();
} // IsWPressed


bool InputManager::IsSPressed() const
{
    return m_Input->IsSPressed();
} // IsSPressed


bool InputManager::IsAPressed() const
{
    return m_Input->IsAPressed();
} // IsAPressed


bool InputManager::IsDPressed() const
{
    return m_Input->IsDPressed();
} // IsDPressed


bool InputManager::IsZPressed() const
{
    return m_Input->IsZPressed();
} // IsZPressed


bool InputManager::IsXPressed() const
{
    return m_Input->IsXPressed();
} // IsXPressed