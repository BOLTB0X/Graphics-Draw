#include "Pch.h"
#include "Light.h"
// Framework
#include "Position.h"


Light::Light()
    : m_color(1.0f, 1.0f, 1.0f, 1.0f),
    m_intensity(1.0f)
{
    m_Position = std::make_unique<Position>();
}

Light::~Light() {}


void Light::Init(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT4 color, float intensity)
{
    SetPosition(pos);

    SetColor(color.x, color.y, color.z, color.w);
    SetIntensity(intensity);

    return;
} // Init


void Light::SetPosition(DirectX::XMFLOAT3 position)
{
    m_Position->SetPosition(position.x, position.y, position.z);
} // SetPosition


void Light::SetColor(float r, float g, float b, float a)
{
    m_color = DirectX::XMFLOAT4(r, g, b, a);
} // SetColor


void Light::SetIntensity(float intensity)
{
    m_intensity = intensity;
} // SetColor


DirectX::XMFLOAT3 Light::GetPosition()
{
    return m_Position->GetPosition();
} // GetPosition


Position* Light::GetPositionPtr()
{
    return m_Position.get();
} // GetPositionPtr