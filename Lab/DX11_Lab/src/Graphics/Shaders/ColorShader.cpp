////////////////////////////////////////////////////////////////////////////////
// Filename: ColorShader.cpp
////////////////////////////////////////////////////////////////////////////////
#include "ColorShader.h"


ColorShader::ColorShader()
{
} // ColorShader


ColorShader::ColorShader(const ColorShader& other)
{
} // ColorShader


ColorShader::~ColorShader()
{
} // ~ColorShader


bool ColorShader::Init(ID3D11Device* device, HWND hwnd)
{
	return BaseShader::Init(device, 
		hwnd, 
		L"./hlsl/Color.vs", 
		L"./hlsl/Color.ps");
} // Init


void ColorShader::Shutdown()
{
	ShutdownShader();

	return;
} // Shutdown


bool ColorShader::Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
	XMMATRIX projectionMatrix)
{
	// 렌더링에 사용할 셰이더 매개변수를 설정
	if (SetShaderParameters(deviceContext, 
		worldMatrix, viewMatrix, projectionMatrix) == false)
	{
		return false;
	}

	RenderShader(deviceContext, indexCount);

	return true;
} // Render


bool ColorShader::InitShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
{
	//ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer = nullptr;
	ID3D10Blob* pixelShaderBuffer = nullptr;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2] = { '\0' };
	unsigned int numElements = 0;
	//D3D11_BUFFER_DESC matrixBufferDesc;

	// 컴파일할 버텍스 셰이더 코드
	if (CompileShaderFromFile(device, hwnd, vsFilename, 
		"ColorVertexShader", "vs_5_0", &vertexShaderBuffer)
		== false)
		return false;

	if (CompileShaderFromFile(device, hwnd, psFilename, 
		"ColorPixelShader", "ps_5_0", &pixelShaderBuffer)
		== false)
		return false;
	
	// 버퍼로부터 정점 셰이더를 생성
	device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	// 버퍼로부터 픽셀 셰이더를 생성
	device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);

	// 레이아웃에 있는 요소의 개수를 셈
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);
	const char* names[] = { "POSITION", "COLOR" };
	DXGI_FORMAT formats[] = { DXGI_FORMAT_R32G32B32_FLOAT, DXGI_FORMAT_R32G32B32A32_FLOAT };

	// 버텍스 입력 레이아웃을 생성
	if (BaseShader::CreateVertexInputLayout(device, vertexShaderBuffer, names, formats, numElements)
		== false) 
		return false;

	vertexShaderBuffer->Release();
	pixelShaderBuffer->Release();

	if (!CreateMatrixBuffer(device))
		return false;
	return true;
} // InitShader


void ColorShader::ShutdownShader()
{
	BaseShader::ShutdownShader();
	return;
} // ShutdownShader


bool ColorShader::SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
	XMMATRIX projectionMatrix)
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

	return true;
} // SetShaderParameters


void ColorShader::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	deviceContext->IASetInputLayout(m_layout);
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
} // RenderShader