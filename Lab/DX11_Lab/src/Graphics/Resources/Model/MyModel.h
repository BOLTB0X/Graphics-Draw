// MyModel.h
#ifndef _MYMODEL_H_
#define _MYMODEL_H_

#include <d3d11.h>
#include <wrl/client.h>
#include <vector>

#include "Base/Common/CommonType.h"

using Microsoft::WRL::ComPtr;

class MyModel {
public:
    MyModel();
    virtual ~MyModel();

    // Zone에서 데이터를 넘겨주면 GPU 버퍼를 생성함
    bool Init(ID3D11Device* device, const std::vector<VertexType>& vertices, const std::vector<unsigned long>& indices);
    void SetTextures(const std::vector<ID3D11ShaderResourceView*>& textures);

    void Render(ID3D11DeviceContext* context);
    int GetIndexCount() const { return m_indexCount; }
    ID3D11ShaderResourceView* GetTexture(int index) const;

private:
    ComPtr<ID3D11Buffer> m_vertexBuffer;
    ComPtr<ID3D11Buffer> m_indexBuffer;

    int m_vertexCount;
    int m_indexCount;

    // 공유 리소스이므로 ComPtr로 참조만 유지
    std::vector<ID3D11ShaderResourceView*> m_textures;
};

#endif