#include "CloudShader.h"
// Common
#include "EngineSettings.h"
#include "EngineHelper.h"


using namespace DirectX;


CloudShader::CloudShader()
	: Shader()
{}


bool CloudShader::Init(ID3D11Device* device, HWND hwnd,
    const std::wstring& vsPath, const std::wstring& psPath)
{
    ID3DBlob* vsBlob = nullptr;
    ID3DBlob* psBlob = nullptr;

    // 셰이더 컴파일
    if (EngineHelper::SuccessCheck(Compile(device, hwnd, vsPath, "main", "vs_5_0", &vsBlob), 
        "CloudShader::Init, vs 컴파일 에러") == false) return false;
    if (EngineHelper::SuccessCheck(Compile(device, hwnd, psPath, "main", "ps_5_0", &psBlob),
        "CloudShader::Init, ps 컴파일 에러") == false) return false;

    // 셰이더 생성
    device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &m_vertexShader);
    device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &m_pixelShader);

    // 입력 레이아웃 생성
    // GeometryArea의 VertexType 에 맞쳐야함
    D3D11_INPUT_ELEMENT_DESC layoutDesc[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };

    device->CreateInputLayout(layoutDesc, 2, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &m_layout);

    vsBlob->Release();
    psBlob->Release();

    // 상수 버퍼 생성 (매트릭스 버퍼 - b0)
    D3D11_BUFFER_DESC matrixBufferDesc = {};
    matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    matrixBufferDesc.ByteWidth = sizeof(MatrixBuffer);
    matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    device->CreateBuffer(&matrixBufferDesc, nullptr, &m_matrixBuffer);

    // 상수 버퍼 생성 (글로벌 버퍼 - b1)
    matrixBufferDesc.ByteWidth = sizeof(GlobalBuffer);
    device->CreateBuffer(&matrixBufferDesc, nullptr, &m_globalBuffer);

    return true;
} // Init


void CloudShader::BindShader(ID3D11DeviceContext* context)
{
    context->IASetInputLayout(m_layout.Get());
    context->VSSetShader(m_vertexShader.Get(), nullptr, 0);
    context->PSSetShader(m_pixelShader.Get(), nullptr, 0);
} // BindShader


void CloudShader::BindConstantBuffers(ID3D11DeviceContext* context)
{
    context->VSSetConstantBuffers(0, 1, m_matrixBuffer.GetAddressOf()); // 행렬(b0)
    context->PSSetConstantBuffers(1, 1, m_globalBuffer.GetAddressOf()); // 레이마칭(b1)
} // BindConstantBuffers


bool CloudShader::UpdateMatrixBuffer(ID3D11DeviceContext* context, XMMATRIX model, XMMATRIX view, XMMATRIX proj)
{
    D3D11_MAPPED_SUBRESOURCE mapped;
    if (FAILED(context->Map(m_matrixBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped))) return false;

    MatrixBuffer* data = (MatrixBuffer*)mapped.pData;
    data->model = XMMatrixTranspose(model);
    data->view = XMMatrixTranspose(view);
    data->projection = XMMatrixTranspose(proj);

    context->Unmap(m_matrixBuffer.Get(), 0);
    return true;
} // UpdateMatrixBuffer


bool CloudShader::UpdateGlobalBuffer(ID3D11DeviceContext* context, float time, float uNoiseRes)
{
    D3D11_MAPPED_SUBRESOURCE mapped;
    if (FAILED(context->Map(m_globalBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped))) return false;

    GlobalBuffer* data = (GlobalBuffer*)mapped.pData;
    data->uTime = time;
    data->uResolution = XMFLOAT2(EngineSettings::SCREEN_WIDTH, EngineSettings::SCREEN_HEIGHT);
    data->uNoiseRes = uNoiseRes;

    context->Unmap(m_globalBuffer.Get(), 0);
    return true;
} // UpdateGlobalBuffer