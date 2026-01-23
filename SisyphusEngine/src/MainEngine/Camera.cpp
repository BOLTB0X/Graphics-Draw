#include "Camera.h"
#include "Frustum.h"
// Framework
#include "Position.h"
// Common
#include "EngineSettings.h"
#include "MathHelper.h"


using namespace DirectX;


Camera::Camera()
    : m_up(XMFLOAT3(0.f, 1.f, 0.f)),
    m_fov(45.0f),
    m_near(EngineSettings::SCREEN_NEAR),
    m_far(EngineSettings::SCREEN_DEPTH),
    m_maxPitch(179.9f),
    m_minPitch(-179.9f)
{
    m_aspect = (float)EngineSettings::SCREEN_WIDTH / (float)EngineSettings::SCREEN_HEIGHT;
    m_Position = std::make_unique<Position>();
    m_Frustum = std::make_unique<Frustum>();
    m_Frustum->Init(m_far);
} // Camera


Camera::~Camera() {} // ~Camera


//void Camera::InitView()
//{
//    XMVECTOR eye = XMLoadFloat3(&GetPosition());
//    XMVECTOR positionVector = XMLoadFloat3(&GetPosition());
//    XMVECTOR forward = GetForwardVector();
//    XMVECTOR up = XMLoadFloat3(&m_up);
//
//    m_viewMatrix = XMMatrixLookAtLH(eye, eye + forward, up);
//} // InitView


//void Camera::InitProjection()
//{
//    m_projectionMatrix = XMMatrixTranspose(XMMatrixPerspectiveFovLH(m_fov, m_aspect, m_near, m_far));
//    m_Frustum->Init(m_far);
//} // InitProjection


void Camera::BuildFrustum()
{
    if (m_Frustum)
        m_Frustum->BuildFrustum(GetViewMatrix(), GetProjectionMatrix());
    
    return;
} // Render


void Camera::SetPosition(DirectX::XMFLOAT3 pos)
{
    m_Position->SetPosition(pos);
} // SetPosition


void Camera::SetPosition(float x, float y, float z)
{
    m_Position->SetPosition(x, y, z);
} // SetPosition


void Camera::SetRotation(DirectX::XMFLOAT3 rot)
{
    m_Position->SetRotation(rot);
} // SetRotation


void Camera::SetRotation(float pitch, float yaw, float roll)
{
    m_Position->SetRotation(pitch, yaw, roll);
} // SetRotation


void Camera::SetLookAt(XMFLOAT3 target)
{
    XMFLOAT3 pos = m_Position->GetPosition();
    XMVECTOR look_vector = XMVectorSet(
        target.x - pos.x,
        target.y - pos.y,
        target.z - pos.z,
        0.f
    );

    look_vector = XMVector3Normalize(look_vector);
    XMFLOAT3 vec;
    XMStoreFloat3(&vec, look_vector);
    SetRotation(MathHelper::VectorToRotation(vec));
} // SetLookAt


void Camera::SetLookAt(float x, float y, float z)
{
    SetLookAt(XMFLOAT3(x, y, z));
} // SetLookAt


void Camera::SetFOV(float fovDegree, int width, int height)
{
    m_fov = XMConvertToRadians(fovDegree);
} // SetFOV


XMFLOAT3 Camera::GetPosition() const
{
    return m_Position->GetPosition();
} // GetPosition


XMFLOAT3 Camera::GetRotation() const
{
    return m_Position->GetRotation();
} // GetRotation


XMVECTOR Camera::GetForwardVector() const
{
    return MathHelper::RotationToVector(GetRotation());
} // GetForwardVector


XMVECTOR Camera::GetRightVector() const
{
    XMFLOAT3 rot = m_Position->GetRotation();
    XMMATRIX rotationMat = XMMatrixRotationRollPitchYaw(
        XMConvertToRadians(rot.x),
        XMConvertToRadians(rot.y),
        XMConvertToRadians(rot.z));

    XMVECTOR right = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
    return XMVector3TransformNormal(right, rotationMat);
} // GetRightVector


XMMATRIX Camera::GetWorldMatrix() const
{
    return m_Position->GetWorldMatrix();
} // GetWorldMatrix


XMMATRIX Camera::GetViewMatrix() const
{
    XMVECTOR eye = XMLoadFloat3(&GetPosition());
    XMVECTOR forward = GetForwardVector();
    XMVECTOR up = XMLoadFloat3(&m_up);

    // 바라보는 지점 = 내 위치 + 앞방향
    XMVECTOR focus = XMVectorAdd(eye, forward);

    return XMMatrixLookAtLH(eye, focus, up);
} // GetViewMatrix


XMMATRIX Camera::GetProjectionMatrix() const
{
    float fovRadian = XMConvertToRadians(m_fov);
    return XMMatrixPerspectiveFovLH(fovRadian, m_aspect, m_near, m_far);
} // GetProjectionMatrix


void Camera::AddRotation(float deltaPitch, float deltaYaw)
{
    XMFLOAT3 rot = m_Position->GetRotation();
    float p = MathHelper::RotationWrap(rot.x + deltaPitch);
    float y = MathHelper::RotationWrap(rot.y + deltaYaw);

    p = MathHelper::clamp<float>(p, m_minPitch, m_maxPitch);
    m_Position->SetRotation(p, y, rot.z);
} // AddRotation


void Camera::AddPitch(float value)
{
    XMFLOAT3 rotation = m_Position->GetRotation();
    float new_pitch_value = MathHelper::RotationWrap(rotation.x + value);
    SetRotation(new_pitch_value, rotation.y, rotation.z);
} // AddPitch


void Camera::AddYaw(float value)
{
    XMFLOAT3 rotation = m_Position->GetRotation();
    float new_yaw_value = MathHelper::RotationWrap(rotation.y + value);
    SetRotation(rotation.x, new_yaw_value, rotation.z);
} // AddYaw


void Camera::AddFOV(float deltaDegree)
{
    m_fov -= deltaDegree;
    if (m_fov < 5.0f) m_fov = 5.0f;  // 최대 확대
    if (m_fov > 90.0f) m_fov = 90.0f;  // 최대 축소

} // AddFOV