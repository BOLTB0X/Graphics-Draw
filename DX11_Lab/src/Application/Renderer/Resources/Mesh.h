#pragma once
#include <d3d11.h>
#include <vector>
#include "Renderer/Resources/VertexTypes.h"

class Mesh
{
public:
    ~Mesh();
    bool Init(
        ID3D11Device* device,
        const std::vector<VertexPN>& vertices,
        const std::vector<uint32_t>& indices
    );

    void Bind(ID3D11DeviceContext* context);

    void Draw(ID3D11DeviceContext* context);

    UINT GetIndexCount() const { return mIndexCount; }

private:
    ID3D11Buffer* mVB = nullptr;
    ID3D11Buffer* mIB = nullptr;
    UINT mIndexCount = 0;
};
