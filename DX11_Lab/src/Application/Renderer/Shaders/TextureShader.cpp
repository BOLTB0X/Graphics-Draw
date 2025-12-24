#include "TextureShader.h"
#include "Renderer/Resources/VertexTypes.h"

TextureShader::~TextureShader() {
    if (m_sampleState) {
        m_sampleState->Release();
        m_sampleState = nullptr;
    }
} // ~TextureShader

bool TextureShader::Init(ID3D11Device* device, HWND hwnd,
    const std::wstring& vsPath, const std::wstring& psPath) {
    ID3DBlob* vsBlob = nullptr;
    ID3DBlob* psBlob = nullptr;

    if (!CompileShader(vsPath, "VS_Main", "vs_5_0", &vsBlob, hwnd)) return false;
    device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &m_vertexShader);

    if (!CompileShader(psPath, "PS_Main", "ps_5_0", &psBlob, hwnd)) return false;
    device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &m_pixelShader);

    D3D11_INPUT_ELEMENT_DESC polygonLayout[3];
    polygonLayout[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 };
    polygonLayout[1] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
    polygonLayout[2] = { "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };

    device->CreateInputLayout(polygonLayout, 3, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &m_layout);

    vsBlob->Release();
    psBlob->Release();

    if (!CreateMatrixBuffer(device)) return false;

    D3D11_SAMPLER_DESC samplerDesc{};
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

    device->CreateSamplerState(&samplerDesc, &m_sampleState);

    return true;
} // Init

bool TextureShader::SetShaderParameters(ID3D11DeviceContext* context,
    const DirectX::XMMATRIX& world,
    const DirectX::XMMATRIX& view,
    const DirectX::XMMATRIX& projection,
    ID3D11ShaderResourceView* textureView) {
    
    MatrixBufferType cb;
    cb.world = DirectX::XMMatrixTranspose(world);
    cb.view = DirectX::XMMatrixTranspose(view);
    cb.projection = DirectX::XMMatrixTranspose(projection);

    if (m_matrixBuffer == nullptr) {
        return false;
    }

    D3D11_MAPPED_SUBRESOURCE mappedResource;
    if (SUCCEEDED(context->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource))) {
        memcpy(mappedResource.pData, &cb, sizeof(MatrixBufferType));
        context->Unmap(m_matrixBuffer, 0);
    }

    context->VSSetConstantBuffers(0, 1, &m_matrixBuffer);

    context->PSSetShaderResources(0, 1, &textureView);
    context->PSSetSamplers(0, 1, &m_sampleState);

    return true;
} // SetShaderParameters

bool TextureShader::Render(ID3D11DeviceContext* context, int indexCount,
    const DirectX::XMMATRIX& world,
    const DirectX::XMMATRIX& view,
    const DirectX::XMMATRIX& projection,
    ID3D11ShaderResourceView* textureView) {

    if (!SetShaderParameters(context, world, view, projection, textureView)) return false;

    context->IASetInputLayout(m_layout);
    context->VSSetShader(m_vertexShader, nullptr, 0);
    context->PSSetShader(m_pixelShader, nullptr, 0);

    context->DrawIndexed(indexCount, 0, 0);

    return true;
} // Render