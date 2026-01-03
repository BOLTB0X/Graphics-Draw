////////////////////////////////////////////////////////////////////////////////
// Filename: ModelMesh.h
////////////////////////////////////////////////////////////////////////////////

/// <summary>
/// 실제 ID3D11Buffer와 Texture 객체를 소유하며, 렌더링 명령을 내리는 클래스
/// </summary>

#ifndef _MODELMESH_H_
#define _MODELMESH_H_

//////////////
// INCLUDES //
//////////////
#include <vector>
#include <string>
#include <directxmath.h>
#include <wrl/client.h>

#include "Common/CommonType.h"
#include "Resources/Texture/Texture.h"

using Microsoft::WRL::ComPtr;

class ModelMesh {
public:
    bool Init(ID3D11Device* device, ID3D11DeviceContext* context, const MeshData& data);
    void Render(ID3D11DeviceContext* context);

private:
    bool InitBuffers(ID3D11Device* device, const MeshData& data);

    ComPtr<ID3D11Buffer> m_vertexBuffer;
    ComPtr<ID3D11Buffer> m_indexBuffer;
    std::vector<std::unique_ptr<Texture>> m_textures;
};

#endif