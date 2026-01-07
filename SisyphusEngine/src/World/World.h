// World/World.h
#pragma once
#include <vector>
#include <memory>
#include "Stone/Stone.h"

#include "Application/ModelManager/ModelManager.h"
#include "Graphics/Camera/Camera.h"

class World {
public:
    World();
	World(const World&) = delete;
    ~World();

    bool Init(ID3D11Device* device, ID3D11DeviceContext* context,
        ModelManager* modelManager, TexturesManager* texManager, int screenWidth, int screenHeight);

    void Frame(float frameTime, bool);
    void Render(ID3D11DeviceContext* context, Shader* shader);

public:
    ActorObject* GetActor(size_t index) const;
    const std::vector<std::unique_ptr<ActorObject>>& GetActors() const;

private:
    std::vector<std::unique_ptr<ActorObject>> m_actors;
    std::unique_ptr<Camera> m_mainCamera;
}; // World