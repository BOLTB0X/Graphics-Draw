////////////////////////////////////////////////////////////////////////////////
// Filename: NormalMapShader.cpp
////////////////////////////////////////////////////////////////////////////////
#include "NormalMapShader.h"


NormalMapShader::NormalMapShader()
	: m_sampleState(nullptr),
	m_lightBuffer(nullptr)
{
} // NormalMapShader


NormalMapShader::NormalMapShader(const NormalMapShader& other)
	: m_sampleState(nullptr),
	m_lightBuffer(nullptr)
{
} // NormalMapShader


NormalMapShader::~NormalMapShader()
{
} // ~NormalMapShader


bool NormalMapShader::Init(ID3D11Device* device, HWND hwnd)
{
	return BaseShader::Init(
		device,
		hwnd,
		L"./hlsl/Normalmap.vs",
		L"./hlsl/Normalmap.ps"
	);
} // Init


void NormalMapShader::Shutdown()
{
	ShutdownShader();
	return;
} // Shutdown


bool NormalMapShader::Render(ID3D11DeviceContext* deviceContext, int indexCount, 
	XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix,
	ID3D11ShaderResourceView* texture1, ID3D11ShaderResourceView* texture2, 
	XMFLOAT3 lightDirection, XMFLOAT4 diffuseColor)
{
	bool result;

	// 렌더링에 사용할 셰이더 매개변수를 설정
	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, texture1, texture2, lightDirection, diffuseColor);
	if (!result)
	{
		return false;
	}

	RenderShader(deviceContext, indexCount);

	return true;
} // Render


bool NormalMapShader::InitShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
{
	HRESULT result;
	ID3D10Blob* vertexShaderBuffer = 0;
	ID3D10Blob* pixelShaderBuffer = 0;
	unsigned int numElements = 0;
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC lightBufferDesc;

	// 컴파일할 버텍스 셰이더 코드
	if (CompileShaderFromFile(device, hwnd, vsFilename, 
		"NormalMapVertexShader", "vs_5_0", &vertexShaderBuffer) == false)
		return false;
	if (CompileShaderFromFile(device, hwnd, psFilename, 
		"NormalMapPixelShader", "ps_5_0", &pixelShaderBuffer) == false)
		return false;
	
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
	const char* names[] = { "POSITION", "TEXCOORD", "NORMAL", "TANGENT", "BINORMAL" };
	DXGI_FORMAT formats[] = {
		DXGI_FORMAT_R32G32B32_FLOAT,
		DXGI_FORMAT_R32G32_FLOAT,
		DXGI_FORMAT_R32G32B32_FLOAT,
		DXGI_FORMAT_R32G32B32_FLOAT,
		DXGI_FORMAT_R32G32B32_FLOAT
	};

	numElements = sizeof(names) / sizeof(names[0]);
	if (BaseShader::CreateVertexInputLayout(device, vertexShaderBuffer, names, formats, numElements)
		== false)
		return false;

	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	// 정점 셰이더에 있는 동적 행렬 상수 버퍼에 대한 descr을 설정
	if (CreateMatrixBuffer(device) == false)
		return false;

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

	result = device->CreateSamplerState(&samplerDesc, &m_sampleState);
	if (FAILED(result))
	{
		return false;
	}

	// 픽셀 셰이더에 있는 광원 동적 상수 버퍼의 설명을 설정
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(LightBufferType);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;

	// 이 클래스 내부에서 정점 셰이더 상수 버퍼에 접근할 수 있도록 상수
	result = device->CreateBuffer(&lightBufferDesc, NULL, &m_lightBuffer);
	if (FAILED(result))
	{
		return false;
	}

	return true;
} // InitShader


void NormalMapShader::ShutdownShader()
{
	if (m_lightBuffer)
	{
		m_lightBuffer->Release();
		m_lightBuffer = 0;
	}

	if (m_sampleState)
	{
		m_sampleState->Release();
		m_sampleState = 0;
	}

	BaseShader::ShutdownShader();
	return;
} // ShutdownShader


bool NormalMapShader::SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix,
	ID3D11ShaderResourceView* texture1, ID3D11ShaderResourceView* texture2, XMFLOAT3 lightDirection, XMFLOAT4 diffuseColor)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	unsigned int bufferNumber;
	LightBufferType* dataPtr2;


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

	// 정점 셰이더에서 상수 버퍼의 위치를 ​​설정
	bufferNumber = 0;

	// 이제 버텍스 셰이더의 상수 버퍼를 업데이트된 값으로 설정
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	//픽셀 셰이더에서 셰이더 텍스처 리소스를 설정
	deviceContext->PSSetShaderResources(0, 1, &texture1);
	deviceContext->PSSetShaderResources(1, 1, &texture2);

	// 쓰기 작업이 가능하도록 광 상수 버퍼를 Lock
	result = deviceContext->Map(m_lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	// 상수 버퍼에 있는 데이터에 대한 포인터를 가져옴
	dataPtr2 = (LightBufferType*)mappedResource.pData;

	// lighting 변수를 상수 버퍼에 복사
	dataPtr2->diffuseColor = diffuseColor;
	dataPtr2->lightDirection = lightDirection;
	dataPtr2->padding = 0.0f;

	deviceContext->Unmap(m_lightBuffer, 0);

	// 픽셀 셰이더에서 광원 상수 버퍼의 위치를 ​​설정
	bufferNumber = 0;

	//  마지막으로 업데이트된 값으로 픽셀 셰이더의 광원 상수 버퍼를 설정
	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_lightBuffer);

	return true;
} // SetShaderParameters


void NormalMapShader::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	deviceContext->IASetInputLayout(m_layout);
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);
	deviceContext->PSSetSamplers(0, 1, &m_sampleState);
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
} // RenderShader