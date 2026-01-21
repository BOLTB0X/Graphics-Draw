#pragma once
#include <d3d11.h>
#include <directxmath.h>
#include <wrl/client.h>
#include <vector>
#include <memory>

class Position;

class GeometryArea {
public:
    GeometryArea();
    GeometryArea(const GeometryArea& other) = delete;
    ~GeometryArea();

    bool Init(ID3D11Device* device);
    void Shutdown();

    void Render(ID3D11DeviceContext* context);

public:
    int GetVertexCount() const { return m_vertexCount; }
    int GetIndexCount() const { return m_indexCount; }

    DirectX::XMMATRIX GetModelMatrix();

    void SetPosition(DirectX::XMFLOAT3);
    void SetScale(float);
    void SetTransform(DirectX::XMFLOAT3);

public:
    struct VertexType {
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT2 texture;
    };


private:
    bool InitBuffers(ID3D11Device* device);

private:
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer;
    std::unique_ptr<Position> m_Position;

    int m_vertexCount;
    int m_indexCount;
}; // GeometryArea