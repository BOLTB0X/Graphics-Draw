// World/Stone/Stone.cpp
#include "Stone.h"

/* default */
/////////////////////////////////////////////////////////////////////

Stone::Stone()
	: ActorObject()
{
} // Stone

Stone::~Stone() {} // ~Stone

bool Stone::Init(Model* model, const std::string& name)
{
    if (ActorObject::Init(model, name) == false)
        return false;
    

    m_transform->SetRotation(0.0f, 0.0f, 0.0f);

    return true;
} // Init


void Stone::Frame(float frameTime)
{
    ActorObject::Frame(frameTime);
} // Frame


void Stone::Render(ID3D11DeviceContext* context, Shader* shader,
    const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& projection)
{
    ActorObject::Render(context, shader, view, projection);
} // Render