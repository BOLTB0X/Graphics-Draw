#include "Camera.h"
#include "Frustum.h"
// Framework
#include "Position.h"
// Common
#include "MathHelper.h"

using namespace DirectX;


Camera::Camera()
    : m_viewMatrix(XMMatrixIdentity()),
    m_projectionMatrix(XMMatrixIdentity()),
	m_fieldOfView(XM_PIDIV4),
	m_screenNear(0.1f),
	m_screenDepth(1000.0f)
{
    m_Position = std::make_unique<Position>();
    m_Frustum = std::make_unique<Frustum>();
} // Camera


Camera::~Camera() {} // ~Camera


void Camera::InitView()
{
    XMFLOAT3 pos = m_Position->GetPosition();
    XMFLOAT3 rot = m_Position->GetRotation();

    XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    XMVECTOR position = XMVectorSet(pos.x, pos.y, pos.z, 0.0f);
    XMVECTOR lookAt = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

    float pitch = MathHelper::ToRadians(rot.x);
    float yaw = MathHelper::ToRadians(rot.y);
    float roll = MathHelper::ToRadians(rot.z);

    XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

    lookAt = XMVector3TransformCoord(lookAt, rotationMatrix);
    up = XMVector3TransformCoord(up, rotationMatrix);

    lookAt = XMVectorAdd(position, lookAt);

    m_viewMatrix = XMMatrixLookAtLH(position, lookAt, up);
} // InitView


void Camera::InitProjection(
    int width, int height,
    float screenNear, float screenDepth)
{
    m_screenNear = screenNear;
    m_screenDepth = screenDepth;
    float aspectRatio = (float)width / (float)height;

    m_projectionMatrix = XMMatrixPerspectiveFovLH(m_fieldOfView, aspectRatio, screenNear, screenDepth);
    m_Frustum->Init(screenDepth);
} // InitProjection


void Camera::Render()
{
    if (m_Frustum)
        m_Frustum->BuildFrustum(m_viewMatrix, m_projectionMatrix);
    
    return;
} // Render


void Camera::SetPosition(float x, float y, float z)
{
    m_Position->SetPosition(x, y, z);
    InitView();
} // SetCameraPosition


void Camera::SetRotation(float x, float y, float z)
{
    m_Position->SetRotation(x, y, z);
    InitView();
} // SetRotation


XMMATRIX Camera::GetWorldMatrix()
{
    return m_Position->GetWorldMatrix();
} // GetWorldMatrix