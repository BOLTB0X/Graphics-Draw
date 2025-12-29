////////////////////////////////////////////////////////////////////////////////
// Filename: Stone.cpp
////////////////////////////////////////////////////////////////////////////////
#include "Stone.h"


Stone::Stone()
    : m_modelResource(nullptr),
    m_position(0.0f, 0.0f, 0.0f),
    m_rotation(0.0f, 0.0f, 0.0f),
    m_scale(1.0f, 1.0f, 1.0f),
    m_worldMatrix(XMMatrixIdentity()),
    m_radius(0.0f)
{
} // Stone

Stone::~Stone()
{
} // ~Stone

void Stone::Init(Model* modelResource, float x, float y, float z, float radius)
{
    m_modelResource = modelResource;
    m_position = XMFLOAT3(x, y, z);
    m_radius = radius;

    UpdateWorldMatrix();
} // Init

void Stone::Update(float rotX, float rotY)
{
    m_rotation.x += rotX;
    m_rotation.y += rotY;


    UpdateWorldMatrix();
} // Update

void Stone::Move(float dx, float dy, float dz)
{
    m_position.x += dx;
    m_position.y += dy;
    m_position.z += dz;

    UpdateWorldMatrix();
} // Move

bool Stone::Render(ID3D11DeviceContext* deviceContext,
    ShaderManager* shaderManager,
    const XMMATRIX& viewMatrix,
    const XMMATRIX& projectionMatrix,
    Light* light)
{
    // 1. 모델 리소스의 버퍼를 파이프라인에 설정 (IASetVertexBuffers 등)
    m_modelResource->Render(deviceContext);

    // 2. 셰이더를 사용하여 실제로 그리기
    // Stone이 가지고 있는 m_worldMatrix를 사용합니다.
    bool result = shaderManager->RenderNormalMapShader(
        deviceContext,
        m_modelResource->GetIndexCount(),
        m_worldMatrix,
        viewMatrix,
        projectionMatrix,
        m_modelResource->GetTexture(0), // 기본 텍스처
        m_modelResource->GetTexture(1), // 노멀 맵
        light->GetDirection(),
        light->GetDiffuseColor()
    );

    return result;
} // Render

XMMATRIX Stone::GetWorldMatrix()
{
    return m_worldMatrix;
} // GetWorldMatrix

void Stone::UpdateWorldMatrix()
{
    XMMATRIX rotationMatrix, translationMatrix, scaleMatrix;

    scaleMatrix = XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z);

    rotationMatrix = XMMatrixRotationRollPitchYaw(m_rotation.x, m_rotation.y, m_rotation.z);

    translationMatrix = XMMatrixTranslation(m_position.x, m_position.y, m_position.z);

    m_worldMatrix = XMMatrixMultiply(scaleMatrix, rotationMatrix);
    m_worldMatrix = XMMatrixMultiply(m_worldMatrix, translationMatrix);
} // UpdateWorldMatrix

bool Stone::CheckCollision(Stone* other)
{
    // 두 구체 중심 간의 거리 계산
    float diffX = m_position.x - other->m_position.x;
    float diffY = m_position.y - other->m_position.y;
    float diffZ = m_position.z - other->m_position.z;

    float distance = sqrt((diffX * diffX) + (diffY * diffY) + (diffZ * diffZ));

    // 거리가 반지름의 합보다 작으면 충돌함
    return distance < (m_radius + other->m_radius);
} // CheckCollision