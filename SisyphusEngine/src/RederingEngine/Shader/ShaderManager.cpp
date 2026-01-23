#include "ShaderManager.h"
// Framework
#include "Shader.h"
// Rendering
#include "Shader/CloudShader.h"
#include "Shader/SunShader.h"
// Common
#include "EngineSettings.h"

using namespace DirectX;


ShaderManager::ShaderManager() {}


ShaderManager::~ShaderManager()
{
    Shutdown();
} // ShaderManager


bool ShaderManager::Init(ID3D11Device* device, HWND hwnd)
{
    auto cloudShader = std::make_unique<CloudShader>();
    if (cloudShader->Init(
        device,
        hwnd,
        EngineSettings::DEFAULT_VS,
        EngineSettings::CLOUD_PS)
        == false) return false;
    m_shaders["Cloud"] = std::move(cloudShader);

    auto sunShader = std::make_unique<SunShader>();
    if (sunShader->Init(
        device,
        hwnd,
        EngineSettings::DEFAULT_VS,
        EngineSettings::SUN_PS)
        == false) return false;

    m_shaders["Sun"] = std::move(sunShader);

    return true;
} // Init


void ShaderManager::Shutdown()
{
    m_shaders.clear();
} // Shutdown


void ShaderManager::UpdateMatrixBuffer(const std::string key, ID3D11DeviceContext* context,
    XMMATRIX model, XMMATRIX view, XMMATRIX proj)
{
    auto it = m_shaders.find(key);
    if (it == m_shaders.end())
        return;
    it->second->UpdateMatrixBuffer(context, model, view, proj);
} // UpdateMatrixBuffer


void ShaderManager::UpdateGlobalBuffer(
    const std::string key,
    ID3D11DeviceContext* context,
    float time, XMFLOAT3 cameraPos, float uNoiseRes)
{
    auto it = m_shaders.find(key);
    if (it == m_shaders.end())
        return;
    it->second->UpdateGlobalBuffer(context, time, cameraPos, uNoiseRes);
} // UpdateGlobalBuffer


void ShaderManager::UpdateLightBuffer(const std::string key,
    ID3D11DeviceContext* context, Light* light)
{
    auto it = m_shaders.find(key);
    if (it == m_shaders.end()) return;

    Shader* shader = it->second.get();
    if (shader->GetShaderType() == ShaderType::Sun)
    {
        static_cast<SunShader*>(shader)->UpdateLightBuffer(context, light);
    }
} // UpdateLightBuffer


void ShaderManager::SetShaders(const std::string key, ID3D11DeviceContext* context)
{
    auto it = m_shaders.find(key);
    if (it == m_shaders.end()) return;

    Shader* shader = it->second.get();
    shader->SetShaders(context);
} //SetShaders


void ShaderManager::SetConstantBuffers(const std::string key,
    ID3D11DeviceContext* context)
{
    auto it = m_shaders.find(key);
    if (it == m_shaders.end()) return;

    Shader* shader = it->second.get();
    ShaderType type = shader->GetShaderType();

    if (type == ShaderType::Cloud)
    {
        ID3D11Buffer* buffer = GetShader<SunShader>("Sun")->GetLightBuffer();
        if (buffer == nullptr) return;
        static_cast<CloudShader*>(shader)->SetConstantBuffers(context, buffer);
    }
    else if (type == ShaderType::Sun)
    {
        static_cast<SunShader*>(shader)->SetConstantBuffers(context);
    }
} // SetConstantBuffers