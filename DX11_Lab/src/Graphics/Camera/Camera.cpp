////////////////////////////////////////////////////////////////////////////////
// Filename: Camera.cpp
////////////////////////////////////////////////////////////////////////////////
#include "Camera.h"


Camera::Camera()
{
	m_positionX = 0.0f;
	m_positionY = 0.0f;
	m_positionZ = 0.0f;

	m_rotationX = 0.0f;
	m_rotationY = 0.0f;
	m_rotationZ = 0.0f;

	m_distance = 10.0f;
	m_viewMatrix = XMMatrixIdentity();
	m_baseViewMatrix = XMMatrixIdentity();
} // Camera


Camera::Camera(const Camera& other)
{
	m_positionX = 0.0f;
	m_positionY = 0.0f;
	m_positionZ = 0.0f;

	m_rotationX = 0.0f;
	m_rotationY = 0.0f;
	m_rotationZ = 0.0f;

	m_distance = 10.0f;
	m_viewMatrix = XMMatrixIdentity();
	m_baseViewMatrix = XMMatrixIdentity();
} // Camera


Camera::~Camera()
{
} // Camera


void Camera::SetPosition(float x, float y, float z)
{
	m_positionX = x;
	m_positionY = y;
	m_positionZ = z;
	return;
} // SetPosition


void Camera::SetRotation(float x, float y, float z)
{
	m_rotationX = x;
	m_rotationY = y;
	m_rotationZ = z;
	return;
} // SetRotation


XMFLOAT3 Camera::GetPosition()
{
	return XMFLOAT3(m_positionX, m_positionY, m_positionZ);
} // 


XMFLOAT3 Camera::GetRotation()
{
	return XMFLOAT3(m_rotationX, m_rotationY, m_rotationZ);
} // GetRotation


void Camera::Render()
{
	XMFLOAT3 up, position, lookAt;
	XMVECTOR upVector, positionVector, lookAtVector;
	float yaw, pitch, roll;
	XMMATRIX rotationMatrix;

	// 업 설정
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;
	upVector = XMLoadFloat3(&up);

	// 카메라의 위치를 ​​월드 상에서 설정
	position.x = m_positionX;
	position.y = m_positionY;
	position.z = m_positionZ;
	positionVector = XMLoadFloat3(&position);

	// 카메라가 기본적으로 향하는 방향을 설정
	lookAt.x = 0.0f;
	lookAt.y = 0.0f;
	lookAt.z = 1.0f;
	lookAtVector = XMLoadFloat3(&lookAt);
	
	// 회전행렬을 위해, yaw(Y축), pitch(X축), roll(Z축) 회전 값을 라디안 단위로 설정
	pitch = m_rotationX * 0.0174532925f;
	yaw = m_rotationY * 0.0174532925f;
	roll = m_rotationZ * 0.0174532925f;
	rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	// 뷰가 원점을 기준으로 올바르게 회전되도록 lookAt 벡터와 up 벡터를 회전 행렬로 변환
	lookAtVector = XMVector3TransformCoord(lookAtVector, rotationMatrix);
	upVector = XMVector3TransformCoord(upVector, rotationMatrix);

	// 회전된 카메라 위치를 시청자의 위치로 이동
	lookAtVector = XMVectorAdd(positionVector, lookAtVector);

	// 마지막으로 업데이트된 세 벡터로부터 뷰 행렬을 생성
	m_viewMatrix = XMMatrixLookAtLH(positionVector, lookAtVector, upVector);

	return;
} // Render


void Camera::GetViewMatrix(XMMATRIX& viewMatrix)
{
	viewMatrix = m_viewMatrix;
	return;
} // GetViewMatrix


void Camera::Zoom(float delta)
{
	// delta가 양수면 in, 음수면 out
	float zoomAmount = delta * 0.01f;

	// 현재 방향(Forward) 계산
	XMVECTOR lookAt = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	float pitch = m_rotationX * 0.0174532925f;
	float yaw = m_rotationY * 0.0174532925f;
	float roll = m_rotationZ * 0.0174532925f;
	XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);
	XMVECTOR forward = XMVector3TransformCoord(lookAt, rotationMatrix);

	// 예상되는 새로운 위치 계산
	XMVECTOR currentPos = XMLoadFloat3(&XMFLOAT3(m_positionX, m_positionY, m_positionZ));
	XMVECTOR nextPos = XMVectorMultiplyAdd(forward, XMVectorReplicate(zoomAmount), currentPos);

	// 원점으로부터의 거리 체크
	float distance = XMVectorGetX(XMVector3Length(nextPos));

	//// 최소 2.0f에서 최대 50.0f 사이로 제한
	if (distance >= 2.0f && distance <= 50.0f)
	{
		XMStoreFloat3((XMFLOAT3*)&m_positionX, nextPos);
	}

	m_positionZ += zoomAmount;

	if (m_positionZ > -1.0f) m_positionZ = -1.0f;
	if (m_positionZ < -100.0f) m_positionZ = -100.0f;
} // Zoom

void Camera::RenderBaseViewMatrix()
{
	XMFLOAT3 up, position, lookAt;
	XMVECTOR upVector, positionVector, lookAtVector;
	float yaw, pitch, roll;
	XMMATRIX rotationMatrix;

	// 업 설정
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;
	upVector = XMLoadFloat3(&up);

	// 카메라의 위치를 ​​월드 상에서 설정
	position.x = m_positionX;
	position.y = m_positionY;
	position.z = m_positionZ;
	positionVector = XMLoadFloat3(&position);

	// 카메라가 기본적으로 향하는 방향을 설정
	lookAt.x = 0.0f;
	lookAt.y = 0.0f;
	lookAt.z = 1.0f;
	lookAtVector = XMLoadFloat3(&lookAt);

	// 회전행렬을 위해, yaw(Y축), pitch(X축), roll(Z축) 회전 값을 라디안 단위로 설정
	pitch = m_rotationX * 0.0174532925f;
	yaw = m_rotationY * 0.0174532925f;
	roll = m_rotationZ * 0.0174532925f;
	rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	// 뷰가 원점을 기준으로 올바르게 회전되도록 lookAt 벡터와 up 벡터를 회전 행렬로 변환
	lookAtVector = XMVector3TransformCoord(lookAtVector, rotationMatrix);
	upVector = XMVector3TransformCoord(upVector, rotationMatrix);

	// 회전된 카메라 위치를 viewe의 위치로 이동
	lookAtVector = XMVectorAdd(positionVector, lookAtVector);

	// 마지막으로 업데이트된 세 벡터로부터 뷰 행렬을 생성
	m_baseViewMatrix = XMMatrixLookAtLH(positionVector, lookAtVector, upVector);

	return;
} // RenderBaseViewMatrix


void Camera::GetBaseViewMatrix(XMMATRIX& viewMatrix)
{
	viewMatrix = m_baseViewMatrix;
	return;
} // GetBaseViewMatrix