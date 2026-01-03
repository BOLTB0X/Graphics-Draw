////////////////////////////////////////////////////////////////////////////////
// Filename: BaseShader.cpp
////////////////////////////////////////////////////////////////////////////////
#include "BaseShader.h"


BaseShader::BaseShader()
	: m_vertexShader(nullptr),
	m_pixelShader(nullptr),
	m_layout(nullptr),
	m_matrixBuffer(nullptr)
{
}; // BaseShader

BaseShader::~BaseShader()
{
} // ~ BaseShader


bool BaseShader::Init(ID3D11Device* device, HWND hwnd,
	const wchar_t* vsPath, const wchar_t* psPath)
{
	bool result;
	wchar_t vsFilename[128];
	wchar_t psFilename[128];
	int error;

	error = wcscpy_s(vsFilename, 128, vsPath);
	if (error != 0)
	{
		return false;
	}

	error = wcscpy_s(psFilename, 128, psPath);
	if (error != 0)
	{
		return false;
	}

	result = InitShader(device, hwnd, vsFilename, psFilename);
	if (!result)
	{
		return false;
	}

	return true;
} // Init


bool BaseShader::CompileShaderFromFile(ID3D11Device* device, HWND hwnd, WCHAR* filename,
	LPCSTR entryPoint, LPCSTR profile, ID3DBlob** shaderBlob)
{
	ID3DBlob* errorMessage = nullptr;
	HRESULT result;

	result = D3DCompileFromFile(filename, NULL, NULL, entryPoint, profile,
		D3D10_SHADER_ENABLE_STRICTNESS, 0, shaderBlob, &errorMessage);

	if (FAILED(result)) {
		if (errorMessage) {
			OutputShaderErrorMessage(errorMessage, hwnd, filename);
		}
		else {
			MessageBoxW(hwnd, filename, L"Missing Shader File", MB_OK);
		}
		return false;
	}
	return true;
} // CompileShaderFromFile


void BaseShader::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
{
	char* compileErrors;
	unsigned long long bufferSize, i;
	ofstream fout;

	compileErrors = (char*)(errorMessage->GetBufferPointer());
	bufferSize = errorMessage->GetBufferSize();

	fout.open("shader-error.txt");

	for (i = 0; i < bufferSize; i++)
	{
		fout << compileErrors[i];
	}

	fout.close();

	errorMessage->Release();
	errorMessage = 0;
	MessageBoxW(hwnd, L"Error compiling shader.  Check shader-error.txt for message.", shaderFilename, MB_OK);

	return;
} // OutputShaderErrorMessage


bool BaseShader::CreateMatrixBuffer(ID3D11Device* device) {
	D3D11_BUFFER_DESC matrixBufferDesc;
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	return SUCCEEDED(device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer));
} // CreateMatrixBuffer

bool BaseShader::CreateVertexInputLayout(
	ID3D11Device* device,
	ID3DBlob* vertexShaderBuffer,
	const char** semanticNames,
	DXGI_FORMAT* formats,
	unsigned int numElements)
{
	D3D11_INPUT_ELEMENT_DESC* polygonLayout = new D3D11_INPUT_ELEMENT_DESC[numElements];

	for (unsigned int i = 0; i < numElements; ++i)
	{
		polygonLayout[i].SemanticName = semanticNames[i];
		polygonLayout[i].SemanticIndex = 0;
		polygonLayout[i].Format = formats[i];
		polygonLayout[i].InputSlot = 0;
		polygonLayout[i].AlignedByteOffset = (i == 0) ? 0 : D3D11_APPEND_ALIGNED_ELEMENT;
		polygonLayout[i].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		polygonLayout[i].InstanceDataStepRate = 0;
	}

	HRESULT result = device->CreateInputLayout(
		polygonLayout,
		numElements,
		vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(),
		&m_layout
	);

	delete[] polygonLayout;

	return SUCCEEDED(result);
} // CreateVertexInputLayout

void BaseShader::ShutdownShader() {
	if (m_matrixBuffer) {
		m_matrixBuffer->Release();
		m_matrixBuffer = 0;
	}
	if (m_layout) {
		m_layout->Release();
		m_layout = 0;
	}
	if (m_pixelShader) {
		m_pixelShader->Release();
		m_pixelShader = 0;
	}
	if (m_vertexShader) {
		m_vertexShader->Release();
		m_vertexShader = 0;
	}
	return;
} // ShutdownShader