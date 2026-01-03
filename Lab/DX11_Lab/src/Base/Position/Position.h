////////////////////////////////////////////////////////////////////////////////
// Filename: Position.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _POSITION_H_
#define _POSITION_H_


//////////////
// INCLUDES //
//////////////
#include <directxmath.h>
#include <math.h>


////////////////////////////////////////////////////////////////////////////////
// Class name: Position
////////////////////////////////////////////////////////////////////////////////
class Position
{
public:
    Position();
    Position(const Position&);
    ~Position();

    // 입력 및 프레임 타임 설정
    void SetFrameTime(float);

    // 위치/회전/크기 설정
    void SetPosition(float x, float y, float z);
    void SetRotation(float x, float y, float z);
    void SetScale(float x, float y, float z);

    // 데이터 Get
    DirectX::XMFLOAT3 GetPosition() const { return m_position; }
    DirectX::XMFLOAT3 GetRotation() const { return m_rotation; }
    DirectX::XMFLOAT3 GetScale() const { return m_scale; }

public:
    // 월드 행렬 가져오기
    DirectX::XMMATRIX GetWorldMatrix();

public:
    void MoveForward(bool);
    void MoveBackward(bool);
    void MoveUpward(bool);
    void MoveDownward(bool);
    void TurnLeft(bool);
    void TurnRight(bool);
    void LookUpward(bool);
    void LookDownward(bool);

private:
    void UpdateWorldMatrix();

private:
    // DirectXMath 구조체 사용
    DirectX::XMFLOAT3 m_position;
    DirectX::XMFLOAT3 m_rotation;
    DirectX::XMFLOAT3 m_scale;

    // 행렬 캐싱 및 상태 관리
    DirectX::XMMATRIX m_worldMatrix;
    bool m_isDirty;

    float m_frameTime;

    // 이동 속도 관련 변수들
    float m_forwardSpeed, m_backwardSpeed;
    float m_upwardSpeed, m_downwardSpeed;
    float m_leftTurnSpeed, m_rightTurnSpeed;
    float m_lookUpSpeed, m_lookDownSpeed;
}; // Position

#endif