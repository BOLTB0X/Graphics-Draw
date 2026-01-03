////////////////////////////////////////////////////////////////////////////////
// Filename: Position.cpp
////////////////////////////////////////////////////////////////////////////////
#include "Position.h"

using namespace DirectX;

Position::Position()
{
	m_position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
	m_worldMatrix = XMMatrixIdentity();
	m_isDirty = true; // 처음 호출 시 계산하도록 true

	m_frameTime = 0.0f;
	m_forwardSpeed = m_backwardSpeed = 0.0f;
	m_upwardSpeed = m_downwardSpeed = 0.0f;
	m_leftTurnSpeed = m_rightTurnSpeed = 0.0f;
	m_lookUpSpeed = m_lookDownSpeed = 0.0f;
} // Position


Position::Position(const Position& other)
{
	m_position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
	m_worldMatrix = XMMatrixIdentity();
	m_isDirty = true; // 처음 호출 시 계산하도록 true

	m_frameTime = 0.0f;
	m_forwardSpeed = m_backwardSpeed = 0.0f;
	m_upwardSpeed = m_downwardSpeed = 0.0f;
	m_leftTurnSpeed = m_rightTurnSpeed = 0.0f;
	m_lookUpSpeed = m_lookDownSpeed = 0.0f;
} // Position


Position::~Position(){} // ~Position


void Position::SetPosition(float x, float y, float z)
{
	m_position = XMFLOAT3(x, y, z);
	m_isDirty = true;
} // SetPosition


void Position::SetRotation(float x, float y, float z)
{
	m_scale = XMFLOAT3(x, y, z);
	m_isDirty = true;
} // SetRotation


XMMATRIX Position::GetWorldMatrix()
{
	if (m_isDirty)
	{
		UpdateWorldMatrix();
	}
	return m_worldMatrix;
} // // GetWorldMatrix


void Position::SetFrameTime(float time)
{
	m_frameTime = time;
	return;
} // SetFrameTime


void Position::MoveForward(bool keydown)
{
	float radians;


	// U프레임 시간과 사용자가 키를 누르고 있는지 여부에 따라 전진 속도를 업데이트
	if (keydown)
	{
		m_forwardSpeed += m_frameTime * 1.0f;
		if (m_forwardSpeed > (m_frameTime * 50.0f))
		{
			m_forwardSpeed = m_frameTime * 50.0f;
		}
	}
	else
	{
		m_forwardSpeed -= m_frameTime * 0.5f;

		if (m_forwardSpeed < 0.0f)
		{
			m_forwardSpeed = 0.0f;
		}
	}

	radians = m_rotation.y * 0.0174532925f;
	m_position.x += sinf(radians) * m_forwardSpeed;
	m_position.z += cosf(radians) * m_forwardSpeed;
	m_isDirty = true;
	return;
} // MoveForward


void Position::MoveBackward(bool keydown)
{
	float radians;


	// 프레임 시간과 사용자가 키를 누르고 있는지 여부에 따라 후진 속도를 업데이트
	if (keydown)
	{
		m_backwardSpeed += m_frameTime * 1.0f;

		if (m_backwardSpeed > (m_frameTime * 50.0f))
		{
			m_backwardSpeed = m_frameTime * 50.0f;
		}
	}
	else
	{
		m_backwardSpeed -= m_frameTime * 0.5f;

		if (m_backwardSpeed < 0.0f)
		{
			m_backwardSpeed = 0.0f;
		}
	}

	// degrees를 radian으로 변환
	radians = m_rotation.y * 0.0174532925f;
	m_position.x -= sinf(radians) * m_backwardSpeed;
	m_position.z -= cosf(radians) * m_backwardSpeed;

	return;
} // MoveBackward


void Position::MoveUpward(bool keydown)
{
	//프레임 시간과 사용자가 키를 누르고 있는지 여부에 따라 업 속도를 업데이트
	if (keydown)
	{
		m_upwardSpeed += m_frameTime * 1.5f;

		if (m_upwardSpeed > (m_frameTime * 15.0f))
		{
			m_upwardSpeed = m_frameTime * 15.0f;
		}
	}
	else
	{
		m_upwardSpeed -= m_frameTime * 0.5f;

		if (m_upwardSpeed < 0.0f)
		{
			m_upwardSpeed = 0.0f;
		}
	}

	m_position.y += m_upwardSpeed;

	return;
} // MoveUpward


void Position::MoveDownward(bool keydown)
{
	// 프레임 시간과 사용자가 키를 누르고 있는지 여부에 따라 업 속도를 업데이트
	if (keydown)
	{
		m_downwardSpeed += m_frameTime * 1.5f;

		if (m_downwardSpeed > (m_frameTime * 15.0f))
		{
			m_downwardSpeed = m_frameTime * 15.0f;
		}
	}
	else
	{
		m_downwardSpeed -= m_frameTime * 0.5f;

		if (m_downwardSpeed < 0.0f)
		{
			m_downwardSpeed = 0.0f;
		}
	}

	m_position.y -= m_downwardSpeed;

	return;
} // MoveDownward


void Position::TurnLeft(bool keydown)
{
	// 프레임 시간과 사용자가 키를 누르고 있는지 여부에 따라 좌회전 속도를 업데이트
	if (keydown)
	{
		m_leftTurnSpeed += m_frameTime * 5.0f;

		if (m_leftTurnSpeed > (m_frameTime * 150.0f))
		{
			m_leftTurnSpeed = m_frameTime * 150.0f;
		}
	}
	else
	{
		m_leftTurnSpeed -= m_frameTime * 3.5f;

		if (m_leftTurnSpeed < 0.0f)
		{
			m_leftTurnSpeed = 0.0f;
		}
	}

	m_rotation.y -= m_leftTurnSpeed;
	if (m_rotation.y < 0.0f)
	{
		m_rotation.y += 360.0f;
	}

	return;
} // TurnLeft


void Position::TurnRight(bool keydown)
{
	// 프레임 시간과 사용자가 키를 누르고 있는지 여부에 따라 우회전 속도를 업데이트
	if (keydown)
	{
		m_rightTurnSpeed += m_frameTime * 5.0f;

		if (m_rightTurnSpeed > (m_frameTime * 150.0f))
		{
			m_rightTurnSpeed = m_frameTime * 150.0f;
		}
	}
	else
	{
		m_rightTurnSpeed -= m_frameTime * 3.5f;

		if (m_rightTurnSpeed < 0.0f)
		{
			m_rightTurnSpeed = 0.0f;
		}
	}

	m_rotation.y += m_rightTurnSpeed;
	if (m_rotation.y > 360.0f)
	{
		m_rotation.y -= 360.0f;
	}

	return;
} // TurnRight


void Position::LookUpward(bool keydown)
{
	// 프레임 시간과 사용자가 키를 누르고 있는지 여부에 따라 위쪽 회전 속도를 업데이트
	if (keydown)
	{
		m_lookUpSpeed += m_frameTime * 7.5f;

		if (m_lookUpSpeed > (m_frameTime * 75.0f))
		{
			m_lookUpSpeed = m_frameTime * 75.0f;
		}
	}
	else
	{
		m_lookUpSpeed -= m_frameTime * 2.0f;

		if (m_lookUpSpeed < 0.0f)
		{
			m_lookUpSpeed = 0.0f;
		}
	}

	m_rotation.x -= m_lookUpSpeed;
	if (m_rotation.x > 90.0f)
	{
		m_rotation.x = 90.0f;
	}

	return;
} // LookUpward


void Position::LookDownward(bool keydown)
{
	// 프레임 시간과 사용자가 키를 누르고 있는지 여부에 따라 하강 회전 속도를 업데이트
	if (keydown)
	{
		m_lookDownSpeed += m_frameTime * 7.5f;

		if (m_lookDownSpeed > (m_frameTime * 75.0f))
		{
			m_lookDownSpeed = m_frameTime * 75.0f;
		}
	}
	else
	{
		m_lookDownSpeed -= m_frameTime * 2.0f;

		if (m_lookDownSpeed < 0.0f)
		{
			m_lookDownSpeed = 0.0f;
		}
	}

	m_rotation.x += m_lookDownSpeed;
	if (m_rotation.x < -90.0f)
	{
		m_rotation.x = -90.0f;
	}

	return;
} // LookDownward


void Position::UpdateWorldMatrix()
{
	XMMATRIX s = XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z);

	XMMATRIX r = XMMatrixRotationRollPitchYaw(
		m_rotation.x * 0.0174532925f,
		m_rotation.y * 0.0174532925f,
		m_rotation.z * 0.0174532925f
	);

	XMMATRIX t = XMMatrixTranslation(m_position.x, m_position.y, m_position.z);

	m_worldMatrix = s * r * t;
	m_isDirty = false;
} // UpdateWorldMatrix