// Framework/ActorObject.cpp
#include "ActorObject.h"
#include "Common/EngineHelper.h"

/* default */
/////////////////////////////////////////////////////////////////

ActorObject::ActorObject()
	:m_name(""),
	m_model(nullptr),
	m_transform(std::make_unique<Position>())
{
} // ActorObject

ActorObject::~ActorObject() {}


bool ActorObject::Init(Model* model, const std::string& name)
{
    if (model == nullptr) return false;

    m_model = model;
    m_name = name;

	m_transform->SetPosition(0.0f, 0.0f, 0.0f);
	m_transform->SetRotation(0.0f, 0.0f, 0.0f);
    m_transform->SetScale(1.0f);

    return true;
} // Init


void ActorObject::Frame(float frameTime)
{
    if (m_transform)
    {
        m_transform->SetFrameTime(frameTime);
    }

    return;
} // Frame


void ActorObject::Render(
    ID3D11DeviceContext* context, Shader* shader,
    const XMMATRIX& view, const XMMATRIX& projection)
{
    if (EngineHelper::SuccessCheck(
        m_model, "ActorObject::Render -> m_Model이 nullptr") == false) return;
    if (EngineHelper::SuccessCheck(
        context, "ActorObject::Render -> context가 nullptr") == false) return;
    if (EngineHelper::SuccessCheck(
        shader, "ActorObject::Render -> shader가 nullptr") == false) return;

    shader->UpdateMatrixBuffer(context, m_transform->GetWorldMatrix(), view, projection);

    shader->Bind(context);
    m_model->Render(context);
} // Render
