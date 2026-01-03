// src/Framework/ActorObject.h
#ifndef _ACTOROBJECT_H_
#define _ACTOROBJECT_H_

#include <directxmath.h>
#include "Base/Position/Position.h"
#include "Base/Common/CommonType.h"

#include "Graphics/Resources/Model/MyModel.h"
#include "Graphics/Shaders/ShaderManager.h"

using namespace DirectX;

class ActorObject
{
protected:
    // 모든 객체는 위치 정보와 모델 리소스를 가짐
    Position                m_position;
    std::shared_ptr<MyModel>  m_model;


public:
    ActorObject();
    virtual ~ActorObject();

public:
    virtual bool Init(std::shared_ptr<MyModel> model) = 0;
    virtual void Update(float) = 0;
    virtual void Render(ID3D11DeviceContext*) = 0;

    std::shared_ptr<MyModel> GetModel() const { return m_model; }
    Position& GetPosition() { return m_position; }
}; // ActorObject

#endif