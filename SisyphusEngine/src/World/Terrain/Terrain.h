// World/Terrain/Terrain.h
#pragma once
#include "Framework/Actor/ActorObject.h"
#include "Graphics/Model/Model.h"


class Terrain : public ActorObject {
public:
    Terrain();
	Terrain(const Terrain&) = delete;
    virtual ~Terrain() = default;

    bool Init(ID3D11Device*, ID3D11DeviceContext*, Model*);

    // 실제 렌더링
    virtual void Render(ID3D11DeviceContext*, Shader*, const XMMATRIX&, const XMMATRIX&) override;

    float GetHeightAt(float x, float z);
private:
    Model* m_modelRef = nullptr;

    struct TerrainVertex {
        DirectX::XMFLOAT3 pos;
    };
    std::vector<TerrainVertex> m_cpuVertices;
    std::vector<unsigned int> m_cpuIndices;
}; // Terrain