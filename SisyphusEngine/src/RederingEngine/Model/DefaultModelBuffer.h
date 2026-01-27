#pragma once
#include <d3d11.h>
#include <directxmath.h>
#include <wrl/client.h>
#include <vector>
#include <memory>
#include <vector>


enum class DefaultModelType {
    Quad,
    Cube,
    Sphere
}; // DefaultModelType


class DefaultModelBuffer {
public:
    struct VertexType {
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT2 texture;
        DirectX::XMFLOAT3 normal;
    };

public:
    DefaultModelBuffer();
    DefaultModelBuffer(const DefaultModelBuffer& other) = delete;
    ~DefaultModelBuffer();

    bool Init(ID3D11Device* device, DefaultModelType);
    void Shutdown();

    void Render(ID3D11DeviceContext* context);

public:
    int GetVertexCount() const { return m_vertexCount; }
    int GetIndexCount() const { return m_indexCount; }

private:
    bool InitBuffers(ID3D11Device* device, std::vector<VertexType>, std::vector<unsigned long>);

    void CreateQuad(std::vector<VertexType>&, std::vector<unsigned long>&, float width = 2.0f, float height = 2.0f);
    void CreateCube(std::vector<VertexType>&, std::vector<unsigned long>&, float width = 2.0f, float height = 2.0f, float depth = 2.0f);
    void CreateSphere(std::vector<VertexType>&, std::vector<unsigned long>&, float radius = 2.0f, int sliceCount = 20, int stackCount = 20);

private:
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer;

    int m_vertexCount;
    int m_indexCount;
}; // DefaultModelBuffer