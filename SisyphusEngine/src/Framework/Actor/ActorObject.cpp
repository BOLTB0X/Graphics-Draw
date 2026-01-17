// Framework/ActorObject.cpp
#include "ActorObject.h"
#include "Shader/Shader.h"
#include "Position/Position.h"
// Graphics
#include "Shader/ActorsShader.h"
#include "Model/MeshModel.h"
#include "Camera/Camera.h"
// Common
#include "EngineHelper.h"

/* default */
/////////////////////////////////////////////////////////////////

ActorObject::ActorObject()
    :actor_name(""),
    actor_Shader(nullptr),
    actor_Model(nullptr)
{
    actor_Position = std::make_unique<Position>();
} // ActorObject


ActorObject::~ActorObject()
{
    Shutdown();
} // ~ActorObject


bool ActorObject::Init(MeshModel* model, ActorsShader* shader, const std::string& name)
{
    if (model == nullptr || shader == nullptr) return false;

    actor_Model = model;
    actor_name = name;
    actor_Shader = shader;

	actor_Position->SetPosition(0.0f, 0.0f, 0.0f);
	actor_Position->SetRotation(0.0f, 0.0f, 0.0f);
    actor_Position->SetScale(1.0f);

    return true;
} // Init

void ActorObject::Shutdown()
{
    if (actor_Shader) actor_Shader = nullptr;
    if (actor_Model)
    {
        actor_Model = nullptr;
	}


    actor_Position.reset();
    return;
} // Shutdown


void ActorObject::Frame(float frameTime)
{
    if (actor_Position)
    {
        actor_Position->SetFrameTime(frameTime);
    }

    return;
} // Frame


void ActorObject::Render(ID3D11DeviceContext* context, Camera* camera)
{
    if (actor_Model == nullptr || actor_Shader == nullptr) return;
    if (EngineHelper::SuccessCheck(context, "ActorObject::Render -> context nullptr")
        == false) return;
    if (EngineHelper::SuccessCheck(camera, "ActorObject::Render -> camera nullptr")
        == false) return;

    if (camera == nullptr) return;

    DirectX::XMMATRIX worldMatrix = actor_Position->GetWorldMatrix();

    actor_Shader->UpdateMatrixBuffer(
        context,
        worldMatrix,
        camera->GetViewMatrix(),
        camera->GetProjectionMatrix()
    );

    actor_Shader->Bind(context);
    auto frustum = camera->GetFrustum();
    if (frustum)
    {
        actor_Model->Render(context, actor_Shader, frustum, worldMatrix);
    }
    else
    {
        actor_Model->Render(context, actor_Shader, nullptr, worldMatrix);
    }
    return;
} // Render


void ActorObject::PlaceOnTerrain(float terrainWorldHeight, float footingBias)
{
    if (actor_Model == nullptr) return;

    float offset = actor_Model->GetBottomOffset();

    // 현재 월드 위치(X, Z) 유지
    DirectX::XMFLOAT3 currentPos = actor_Position->GetPosition();

    // 최종 Y = 지형 높이 + 모델 보정값 - 개별 바이어스
    actor_Position->SetPosition(
        currentPos.x,
        terrainWorldHeight + offset - footingBias,
        currentPos.z
    );
} // PlaceOnTerrain