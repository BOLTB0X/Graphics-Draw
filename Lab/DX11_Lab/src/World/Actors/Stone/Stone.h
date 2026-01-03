////////////////////////////////////////////////////////////////////////////////
// Filename: Stone.h
////////////////////////////////////////////////////////////////////////////////


#ifndef _STONE_H_
#define _STONE_H_


//////////////
// INCLUDES //
//////////////
#include <DirectXMath.h>

#include "Resources/Model/Model.h"
#include "Shaders/ShaderManager.h"
#include "Resources/Light/Light.h"


using namespace DirectX;


class Stone
{
public:
    Stone();
    ~Stone();

    // 초기화: 사용할 모델 리소스와 초기 위치 설정
    void Init(Model* modelResource, float x, float y, float z, float radius);

    // 업데이트: 회전 및 이동 로직 처리
    void Update(float rotX, float rotY);
    void Move(float dx, float dy, float dz);
    bool Render(ID3D11DeviceContext* deviceContext,
        ShaderManager* shaderManager,
        const XMMATRIX& viewMatrix,
        const XMMATRIX& projectionMatrix,
        Light* light);

    // 렌더링에 필요한 정보 제공
    XMMATRIX GetWorldMatrix();
    Model* GetModel() { return m_modelResource; }

    // 충돌 관련
    float GetRadius() { return m_radius; }
    XMFLOAT3 GetPosition() { return XMFLOAT3(m_position.x, m_position.y, m_position.z); }
    bool CheckCollision(Stone* other);

private:
    void UpdateWorldMatrix();

private:
    // 실제 정점 데이터가 담긴 모델 (공유 리소스)
    Model* m_modelResource;

    XMFLOAT3 m_position;
    XMFLOAT3 m_rotation;
    XMFLOAT3 m_scale;

    XMMATRIX m_worldMatrix;

    float m_radius;
}; // Stone


#endif