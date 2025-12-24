#include "Renderer/Resources/Mesh.h"

Mesh::~Mesh() {
    if (mVB) { mVB->Release(); mVB = nullptr; }
    if (mIB) { mIB->Release(); mIB = nullptr; }
} // ~Mesh

bool Mesh::Init(
    ID3D11Device* device,
    const std::vector<VertexPN>& vertices,
    const std::vector<uint32_t>& indices
)
{
    mIndexCount = (UINT)indices.size();

    D3D11_BUFFER_DESC vbDesc{};
    vbDesc.Usage = D3D11_USAGE_DEFAULT;
    vbDesc.ByteWidth = UINT(sizeof(VertexPN) * vertices.size());
    vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA vbData{};
    vbData.pSysMem = vertices.data();

    device->CreateBuffer(&vbDesc, &vbData, &mVB);

    D3D11_BUFFER_DESC ibDesc{};
    ibDesc.Usage = D3D11_USAGE_DEFAULT;
    ibDesc.ByteWidth = UINT(sizeof(uint32_t) * indices.size());
    ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

    D3D11_SUBRESOURCE_DATA ibData{};
    ibData.pSysMem = indices.data();

    device->CreateBuffer(&ibDesc, &ibData, &mIB);

    return true;
} // Init

void Mesh::Bind(ID3D11DeviceContext* context)
{
    UINT stride = sizeof(VertexPN);
    UINT offset = 0;

    context->IASetVertexBuffers(0, 1, &mVB, &stride, &offset);
    context->IASetIndexBuffer(mIB, DXGI_FORMAT_R32_UINT, 0);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
} // Bind

void Mesh::Draw(ID3D11DeviceContext* context)
{
    Bind(context);
    context->DrawIndexed(mIndexCount, 0, 0);
} // Draw
