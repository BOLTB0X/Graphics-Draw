////////////////////////////////////////////////////////////////////////////////
// Filename: TextureShader.cpp
////////////////////////////////////////////////////////////////////////////////
#include "TextureShader.h"

TextureShader::TextureShader()
	: m_sampleState(nullptr)
{
} // TextureShader


TextureShader::TextureShader(const TextureShader& other)
	: m_sampleState(nullptr)
{
} // TextureShader


TextureShader::~TextureShader()
{
} // TextureShader


bool TextureShader::Init(ID3D11Device* device, HWND hwnd)
{
	return BaseShader::Init(device,
		hwnd,
		L"./hlsl/Texture.vs",
		L"./hlsl/Texture.ps");
} // Init


void TextureShader::Shutdown()
{
	ShutdownShader();

	return;
} // Shutdown


bool TextureShader::Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
	XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture)
{

	// 렌더링에 사용할 셰이더 매개변수를 설정
	if (SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, texture)
		== false)
	{
		return false;
	}

	RenderShader(deviceContext, indexCount);

	return true;
} // Render


bool TextureShader::InitShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
{
	HRESULT result;
	ID3D10Blob* vertexShaderBuffer = 0;
	ID3D10Blob* pixelShaderBuffer = 0;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	unsigned int numElements = 0;
	D3D11_SAMPLER_DESC samplerDesc;


	// 컴파일할 버텍스 셰이더 코드
	if (CompileShaderFromFile(device, hwnd, vsFilename, 
		"TextureVertexShader", "vs_5_0", & vertexShaderBuffer)
		== false) return false;

	if (CompileShaderFromFile(device, hwnd, psFilename,
		"TexturePixelShader", "ps_5_0", &pixelShaderBuffer)
		== false) return false;

	// 버퍼로부터 정점 셰이더를 생성
	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	if (FAILED(result))
	{
		return false;
	}

	// 버퍼로부터 픽셀 셰이더를 생성
	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
	if (FAILED(result))
	{
		return false;
	}

	// vertex input layout description 생성
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);
	const char* names[] = { "POSITION", "TEXCOORD" };
	DXGI_FORMAT formats[] = { DXGI_FORMAT_R32G32B32_FLOAT, DXGI_FORMAT_R32G32_FLOAT };

	if (BaseShader::CreateVertexInputLayout(device, vertexShaderBuffer, names, formats, numElements)
		== false)
		return false;


	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;


	if (CreateMatrixBuffer(device) == false)
		return false;

	// texture sampler state description 설정
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// texture sampler state 생성
	result = device->CreateSamplerState(&samplerDesc, &m_sampleState);
	if (FAILED(result))
	{
		return false;
	}

	return true;
} // InitShader


void TextureShader::ShutdownShader()
{
	if (m_sampleState)
	{
		m_sampleState->Release();
		m_sampleState = 0;
	}

	BaseShader::ShutdownShader();

	return;
} // ShutdownShader


bool TextureShader::SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
	XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	unsigned int bufferNumber;


	// 셰이더 처리를 위해 행렬을 전치
	worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projectionMatrix = XMMatrixTranspose(projectionMatrix);

	result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	// 상수 버퍼에 있는 데이터에 대한 포인터를 가져옴
	dataPtr = (MatrixBufferType*)mappedResource.pData;

	// 행렬들을 상수 버퍼에 복사
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;

	deviceContext->Unmap(m_matrixBuffer, 0);

	bufferNumber = 0;

	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);
	deviceContext->PSSetShaderResources(0, 1, &texture);

	return true;
} // SetShaderParameters


void TextureShader::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	deviceContext->IASetInputLayout(m_layout);
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);
	deviceContext->PSSetSamplers(0, 1, &m_sampleState);
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
} // RenderShader