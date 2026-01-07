// Framework/ActorObject.h
#pragma once
#include <memory>
#include <string>

#include "Base/Position.h"
#include "Graphics/Model/Model.h"
#include "Graphics/Shader/Shader.h"

class ActorObject {
public:
    ActorObject();
    ActorObject(const ActorObject&) = delete;
    virtual ~ActorObject();

    virtual bool Init(Model*, const std::string&);
    virtual void Frame(float);
    virtual void Render(ID3D11DeviceContext*, Shader*, const XMMATRIX&, const XMMATRIX&);

public:
    Position* GetTransform() const { return m_transform.get(); }
    const std::string& GetName() const { return m_name; }

protected:
    std::string m_name;
    Model* m_model;
    std::unique_ptr<Position> m_transform;
}; // ActorObject