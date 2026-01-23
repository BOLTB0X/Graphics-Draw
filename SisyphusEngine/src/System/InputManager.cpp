#include "InputManager.h"
//
#include <Input.h>
// Framework
#include "Position.h"
// Enbgine
#include "Camera.h"
// Common
#include "MathHelper.h"


using namespace DirectX;


InputManager::InputManager()
	: m_deltaX(0.0f),
	m_deltaY(0.0f),
    m_sensitivity(0.05f)
{
} // InputManager


InputManager::~InputManager()
{
} // ~InputManager


bool InputManager::Init(std::shared_ptr<Input> input)
{
    if (input == nullptr) return false;
    m_Input = input; // 참조
	return true;
} // Init


bool InputManager::Frame(float deltaTime, Camera* camera, bool isCameraLocked)
{
    if (m_Input == nullptr || camera == nullptr)
        return false;

    if (IsEscapePressed()) return false;

    UpdateMouseDelta(); 

    if (isCameraLocked) return true;

    if (IsMouseLPressed())
        HandleRotation(camera->GetPositionPtr());

    HandleMovement(deltaTime, camera);

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


void InputManager::HandleMovement(float deltaTime, Camera* camera)
{
    Position* camPos = camera->GetPositionPtr();
    camPos->SetFrameTime(deltaTime);

    //if (IsMouseLPressed())
    //{
    //    XMFLOAT3 rot = camPos->GetRotation();
    //    camPos->SetRotation(rot.x + m_deltaY, rot.y + m_deltaX, rot.z);
    //}

    // 키보드 이동
    camPos->MoveForward(IsWPressed());
    camPos->MoveBackward(IsSPressed());
    camPos->TurnLeft(IsAPressed());
    camPos->TurnRight(IsDPressed());
    camPos->MoveUpward(IsZPressed());
    camPos->MoveDownward(IsXPressed());

    // 마우스 휠
    float wheelDelta = static_cast<float>(m_Input->GetMouseWheelDelta());
    if (wheelDelta != 0.0f)
    {
        float fovDelta = wheelDelta * 0.05f;
        camera->AddFOV(fovDelta);
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