#include "Pch.h"
#include "ShaderManager.h"
// Framework
#include "Shader.h"
// Rendering
#include "Shader/CloudShader.h"
#include "Shader/SunShader.h"
#include "Shader/BicubicShader.h"
#include "Shader/SkyShader.h"
// Common
#include "ConstantHelper.h"

using namespace DirectX;


ShaderManager::ShaderManager() {}


ShaderManager::~ShaderManager()
{
    Shutdown();
} // ShaderManager


bool ShaderManager::Init(ID3D11Device* device, HWND hwnd)
{
    auto cloudShader = std::make_unique<CloudShader>();
    if (cloudShader->Init(device, hwnd,
        ConstantHelper::DEFAULT_VS,
        ConstantHelper::CLOUD_PS)
        == false) return false;
    m_shaders[ShaderKeys::Cloud] = std::move(cloudShader);

    auto sunShader = std::make_unique<SunShader>();
    if (sunShader->Init(device, hwnd,
        ConstantHelper::DEFAULT_VS,
        ConstantHelper::SUN_PS)
        == false) return false;

    m_shaders[ShaderKeys::Sun] = std::move(sunShader);

    auto biShader = std::make_unique<BicubicShader>();
    if (biShader->Init(device, hwnd,
        ConstantHelper::QUAD_VS,
        ConstantHelper::BICUBIC_PS)
        == false) return false;

    m_shaders[ShaderKeys::Bicubic] = std::move(biShader);

    auto skyShader = std::make_unique<SkyShader>();
    if (skyShader->Init(device, hwnd,
        ConstantHelper::CUBE_VS,
        ConstantHelper::SKY_PS)
        == false) return false;

    m_shaders[ShaderKeys::Sky] = std::move(skyShader);
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
    float time, float frame, XMFLOAT3 cameraPos, float iNoiseRes)
{
    auto it = m_shaders.find(key);
    if (it == m_shaders.end())
        return;
    it->second->UpdateGlobalBuffer(context, time, frame, cameraPos, iNoiseRes);
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

    if (type == ShaderType::Cloud || type == ShaderType::Sky)
    {
        ID3D11Buffer* buffer = GetShader<SunShader>(ShaderKeys::Sun)->GetLightBuffer();
        if (buffer == nullptr) return;
        if (type == ShaderType::Cloud)
            static_cast<CloudShader*>(shader)->SetConstantBuffers(context, buffer);
        else
            static_cast<SkyShader*>(shader)->SetConstantBuffers(context, buffer);
    }
    else if (type == ShaderType::Sun)
    {
        static_cast<SunShader*>(shader)->SetConstantBuffers(context);
    }
} // SetConstantBuffers