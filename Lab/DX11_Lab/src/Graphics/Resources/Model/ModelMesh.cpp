////////////////////////////////////////////////////////////////////////////////
// Filename: ModelMesh.cpp
////////////////////////////////////////////////////////////////////////////////

/// <summary>
/// 실제 ID3D11Buffer와 Texture 객체를 소유하며, 렌더링 명령을 내리는 클래스
/// </summary>

#include "ModelMesh.h"

bool ModelMesh::Init(ID3D11Device* device, ID3D11DeviceContext* context, const ModelData& data)
{
    if (!InitBuffers(device, data)) return false;

    // 반복문을 통한 유연한 텍스처 로드 (Vector 기반)
    for (const auto& path : data.textureFilenames) {
        auto tex = std::make_unique<Texture>();
        if (tex->Init(device, context, const_cast<char*>(path.c_str()))) {
            m_textures.push_back(std::move(tex));
        }
    }
    return true;
} // init

void ModelMesh::Render(ID3D11DeviceContext* context) {
    unsigned int stride = sizeof(VertexType);
    unsigned int offset = 0;
    context->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);
    context->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

bool ModelMesh::InitBuffers(ID3D11Device* device, const MeshData& data) {

}