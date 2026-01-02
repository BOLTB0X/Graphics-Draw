// MyModel.cpp
#include "MyModel.h"

MyModel::MyModel()
	: m_vertexCount(0),
	m_indexCount(0) {
}


MyModel::~MyModel() { }


bool MyModel::Init(
    ID3D11Device* device,
    const std::vector<VertexType>& vertices,
    const std::vector<unsigned long>& indices)
{
    m_vertexCount = static_cast<int>(vertices.size());
    m_indexCount = static_cast<int>(indices.size());

    // 1. Vertex Buffer 생성
    D3D11_BUFFER_DESC vertexBufferDesc = {};
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA vData = {};
    vData.pSysMem = vertices.data();

    HRESULT hr = device->CreateBuffer(&vertexBufferDesc, &vData, m_vertexBuffer.GetAddressOf());
    if (FAILED(hr))
        return false;

    // 2. Index Buffer 생성
    D3D11_BUFFER_DESC indexBufferDesc = {};
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA iData = {};
    iData.pSysMem = indices.data();

    hr = device->CreateBuffer(&indexBufferDesc, &iData, m_indexBuffer.GetAddressOf());
    if (FAILED(hr))
        return false;

    return true;
} // Init


void MyModel::SetTextures(const std::vector<ID3D11ShaderResourceView*>& textures) {
    m_textures = textures;
} // SetTextures


ID3D11ShaderResourceView* MyModel::GetTexture(int index) const {
    if (index < 0 || index >= m_textures.size())
        return nullptr;
    return m_textures[index];
} // GetTexture


void MyModel::Render(ID3D11DeviceContext* context) {
    unsigned int stride = sizeof(VertexType);
    unsigned int offset = 0;

    context->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);
    context->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
} // Render