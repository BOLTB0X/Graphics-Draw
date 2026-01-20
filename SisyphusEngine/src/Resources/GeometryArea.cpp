#include "GeometryArea.h"


GeometryArea::GeometryArea()
    : m_vertexCount(4),
    m_indexCount(6)
{}

GeometryArea::~GeometryArea() {}


bool GeometryArea::Init(ID3D11Device* device)
{
    return InitBuffers(device);
} // Init


bool GeometryArea::InitBuffers(ID3D11Device* device)
{
    // 레이마칭을 위해 -1 ~ 1 범위를 덮는 사각형
    VertexType vertices[4];
    vertices[0].position = DirectX::XMFLOAT3(-1.0f, 1.0f, 0.0f); // 좌상단
    vertices[0].texture = DirectX::XMFLOAT2(0.0f, 0.0f);

    vertices[1].position = DirectX::XMFLOAT3(1.0f, 1.0f, 0.0f); // 우상단
    vertices[1].texture = DirectX::XMFLOAT2(1.0f, 0.0f);

    vertices[2].position = DirectX::XMFLOAT3(-1.0f, -1.0f, 0.0f); // 좌하단
    vertices[2].texture = DirectX::XMFLOAT2(0.0f, 1.0f);

    vertices[3].position = DirectX::XMFLOAT3(1.0f, -1.0f, 0.0f); // 우하단
    vertices[3].texture = DirectX::XMFLOAT2(1.0f, 1.0f);

    unsigned long indices[6] = { 0, 1, 2, 2, 1, 3 };

    // Vertex Buffer 생성
    D3D11_BUFFER_DESC vbd = {};
    vbd.Usage = D3D11_USAGE_DEFAULT;
    vbd.ByteWidth = sizeof(VertexType) * m_vertexCount;
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA vinit = { vertices };
    if (FAILED(device->CreateBuffer(&vbd, &vinit, &m_vertexBuffer))) return false;

    // Index Buffer 생성
    D3D11_BUFFER_DESC ibd = {};
    ibd.Usage = D3D11_USAGE_DEFAULT;
    ibd.ByteWidth = sizeof(unsigned long) * m_indexCount;
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;

    D3D11_SUBRESOURCE_DATA iinit = { indices };
    if (FAILED(device->CreateBuffer(&ibd, &iinit, &m_indexBuffer))) return false;

    return true;
} // InitBuffers


void GeometryArea::Render(ID3D11DeviceContext* context)
{
    unsigned int stride = sizeof(VertexType);
    unsigned int offset = 0;

    context->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);
    context->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
} // Render


void GeometryArea::Shutdown()
{
    m_indexBuffer.Reset();
    m_vertexBuffer.Reset();
} // Shutdown