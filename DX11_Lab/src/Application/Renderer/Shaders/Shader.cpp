#include "Shader.h"

Shader::~Shader() {
    if (m_matrixBuffer) m_matrixBuffer->Release();
    if (m_layout) m_layout->Release();
    if (m_pixelShader) m_pixelShader->Release();
    if (m_vertexShader) m_vertexShader->Release();
} // Shader

bool Shader::CompileShader(const std::wstring& path, const char* entry, const char* profile, ID3DBlob** blob, HWND hwnd) {
    ID3DBlob* errorMsg = nullptr;
    HRESULT hr = D3DCompileFromFile(path.c_str(), nullptr, nullptr, entry, profile,
        D3D10_SHADER_ENABLE_STRICTNESS, 0, blob, &errorMsg);

    if (FAILED(hr)) {
        if (errorMsg) {
            // 셰이더 컴파일 에러 체크용
            char* compileErrors = (char*)(errorMsg->GetBufferPointer());
            std::cerr << "Shader Compile Error: " << compileErrors << std::endl;
            MessageBoxA(hwnd, compileErrors, "Shader Error", MB_OK);
            errorMsg->Release();
        }
        else {
            // 파일 자체가 없는 경우 등등
            MessageBoxW(hwnd, path.c_str(), L"Missing Shader File", MB_OK);
        }
        return false;
    }
    return true;
} // CompileShader

bool Shader::CreateMatrixBuffer(ID3D11Device* device) {
    D3D11_BUFFER_DESC matrixBufferDesc{};
    matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC; // CPU에서 매 프레임 업데이트
    matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
    matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    return SUCCEEDED(device->CreateBuffer(&matrixBufferDesc, nullptr, &m_matrixBuffer));
} // CreateMatrixBuffer