////////////////////////////////////////////////////////////////////////////////
// Filename: TerrainShader.cpp
////////////////////////////////////////////////////////////////////////////////
#include "TerrainShader.h"


TerrainShader::TerrainShader()
{
	//m_vertexShader = 0;
	//m_pixelShader = 0;
	//m_layout = 0;
	//m_matrixBuffer = 0;
	m_sampleState = 0;
	m_lightBuffer = 0;
} // TerrainShader


TerrainShader::TerrainShader(const TerrainShader& other)
{
	m_sampleState = 0;
	m_lightBuffer = 0;
} // TerrainShader


TerrainShader::~TerrainShader()
{
} // ~TerrainShader


bool TerrainShader::Init(ID3D11Device* device, HWND hwnd)
{
	return BaseShader::Init(
		device,
		hwnd,
		L"hlsl/Terrain.vs",
		L"hlsl/Terrain.ps"
	);
} // Init


void TerrainShader::Shutdown()
{
	ShutdownShader();
	return;
} // Shutdown


bool TerrainShader::Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
	XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* normalMap,
	XMFLOAT3 lightDirection, XMFLOAT4 diffuseColor)
{
	bool result;

	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, texture, normalMap, lightDirection, diffuseColor);
	if (!result)
	{
		return false;
	}

	RenderShader(deviceContext, indexCount);
	return true;
} // Render


bool TerrainShader::InitShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
{
	HRESULT result;
	ID3D10Blob* vertexShaderBuffer = 0;
	ID3D10Blob* pixelShaderBuffer = 0;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[6];
	unsigned int numElements;
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC lightBufferDesc;


	// 컴파일할 버텍스 셰이더 코드
	if (CompileShaderFromFile(device, hwnd, vsFilename,
		"TerrainVertexShader", "vs_5_0", &vertexShaderBuffer) == false)
		return false;
	if (CompileShaderFromFile(device, hwnd, psFilename,
		"TerrainPixelShader", "ps_5_0", &pixelShaderBuffer) == false)
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
	const char* names[] = { "POSITION", "TEXCOORD", "NORMAL", "TANGENT", "BINORMAL", "COLOR"};
	DXGI_FORMAT formats[] = {
		DXGI_FORMAT_R32G32B32_FLOAT,
		DXGI_FORMAT_R32G32_FLOAT,
		DXGI_FORMAT_R32G32B32_FLOAT,
		DXGI_FORMAT_R32G32B32_FLOAT,
		DXGI_FORMAT_R32G32B32_FLOAT,
		DXGI_FORMAT_R32G32B32_FLOAT
	};

	numElements = sizeof(names) / sizeof(names[0]);
	if (BaseShader::CreateVertexInputLayout(device, vertexShaderBuffer, names, formats, numElements)
		== false)
		return false;

	// Create the vertex input layout description.
	//polygonLayout[0].SemanticName = "POSITION";
	//polygonLayout[0].SemanticIndex = 0;
	//polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	//polygonLayout[0].InputSlot = 0;
	//polygonLayout[0].AlignedByteOffset = 0;
	//polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	//polygonLayout[0].InstanceDataStepRate = 0;

	//polygonLayout[1].SemanticName = "TEXCOORD";
	//polygonLayout[1].SemanticIndex = 0;
	//polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	//polygonLayout[1].InputSlot = 0;
	//polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	//polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	//polygonLayout[1].InstanceDataStepRate = 0;

	//polygonLayout[2].SemanticName = "NORMAL";
	//polygonLayout[2].SemanticIndex = 0;
	//polygonLayout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	//polygonLayout[2].InputSlot = 0;
	//polygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	//polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	//polygonLayout[2].InstanceDataStepRate = 0;

	//polygonLayout[3].SemanticName = "TANGENT";
	//polygonLayout[3].SemanticIndex = 0;
	//polygonLayout[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	//polygonLayout[3].InputSlot = 0;
	//polygonLayout[3].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	//polygonLayout[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	//polygonLayout[3].InstanceDataStepRate = 0;

	//polygonLayout[4].SemanticName = "BINORMAL";
	//polygonLayout[4].SemanticIndex = 0;
	//polygonLayout[4].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	//polygonLayout[4].InputSlot = 0;
	//polygonLayout[4].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	//polygonLayout[4].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	//polygonLayout[4].InstanceDataStepRate = 0;

	//polygonLayout[5].SemanticName = "COLOR";
	//polygonLayout[5].SemanticIndex = 0;
	//polygonLayout[5].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	//polygonLayout[5].InputSlot = 0;
	//polygonLayout[5].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	//polygonLayout[5].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	//polygonLayout[5].InstanceDataStepRate = 0;

	//// Get a count of the elements in the layout.
	//numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	//// Create the vertex input layout.
	//result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(),
	//	vertexShaderBuffer->GetBufferSize(), &m_layout);
	//if (FAILED(result))
	//{
	//	return false;
	//}

	// Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	//matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	//matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	//matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	//matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	//matrixBufferDesc.MiscFlags = 0;
	//matrixBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	//result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
	//if (FAILED(result))
	//{
	//	return false;
	//}

	// 정점 셰이더에 있는 동적 행렬 상수 버퍼에 대한 descr을 설정
	if (CreateMatrixBuffer(device) == false)
		return false;

	// Create a texture sampler state description.
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// Create the texture sampler state.
	result = device->CreateSamplerState(&samplerDesc, &m_sampleState);
	if (FAILED(result))
	{
		return false;
	}

	// Setup the description of the light dynamic constant buffer that is in the pixel shader.
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(LightBufferType);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the pixel shader constant buffer from within this class.
	result = device->CreateBuffer(&lightBufferDesc, NULL, &m_lightBuffer);
	if (FAILED(result))
	{
		return false;
	}

	return true;
} // InitShader


void TerrainShader::ShutdownShader()
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

	//// Release the matrix constant buffer.
	//if (m_matrixBuffer)
	//{
	//	m_matrixBuffer->Release();
	//	m_matrixBuffer = 0;
	//}

	//// Release the layout.
	//if (m_layout)
	//{
	//	m_layout->Release();
	//	m_layout = 0;
	//}

	//// Release the pixel shader.
	//if (m_pixelShader)
	//{
	//	m_pixelShader->Release();
	//	m_pixelShader = 0;
	//}

	//// Release the vertex shader.
	//if (m_vertexShader)
	//{
	//	m_vertexShader->Release();
	//	m_vertexShader = 0;
	//}

	return;
} // ShutdownShader


bool TerrainShader::SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
	XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* normalMap,
	XMFLOAT3 lightDirection, XMFLOAT4 diffuseColor)
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

	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;

	// 행렬들을 상수 버퍼에 복사
	deviceContext->Unmap(m_matrixBuffer, 0);

	// 정점 셰이더에서 상수 버퍼의 위치를 ​​설정
	bufferNumber = 0;

	// 이제 버텍스 셰이더의 상수 버퍼를 업데이트된 값으로 설정
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	// 픽셀 셰이더에서 셰이더 텍스처 리소스를 설정
	deviceContext->PSSetShaderResources(0, 1, &texture);
	deviceContext->PSSetShaderResources(1, 1, &normalMap);

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

	// 마지막으로 업데이트된 값으로 픽셀 셰이더의 광원 상수 버퍼를 설정
	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_lightBuffer);

	return true;
} // SetShaderParameters


void TerrainShader::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// Set the vertex input layout.
	deviceContext->IASetInputLayout(m_layout);

	// Set the vertex and pixel shaders that will be used to render.
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	// Set the sampler state in the pixel shader.
	deviceContext->PSSetSamplers(0, 1, &m_sampleState);

	// Render the polygon data.
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
} // RenderShader