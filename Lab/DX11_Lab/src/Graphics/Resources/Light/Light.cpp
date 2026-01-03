////////////////////////////////////////////////////////////////////////////////
// Filename: Light.cpp
////////////////////////////////////////////////////////////////////////////////
#include "Light.h"


Light::Light()
	: m_ambientColor(0.0f, 0.0f, 0.0f, 0.0f),
	 m_diffuseColor(0.0f, 0.0f, 0.0f, 0.0f),
	 m_direction(0.0f, 0.0f, 0.0f),
	 m_specularColor(0.0f, 0.0f, 0.0f, 0.0f),
	 m_specularPower(0.0f),
	m_position(0.0f, 0.0f, 0.0f, 1.0f)
{
} // Light


Light::Light(const Light& other)
	: m_ambientColor(0.0f, 0.0f, 0.0f, 0.0f),
	m_diffuseColor(0.0f, 0.0f, 0.0f, 0.0f),
	m_direction(0.0f, 0.0f, 0.0f),
	m_specularColor(0.0f, 0.0f, 0.0f, 0.0f),
	m_specularPower(0.0f),
	m_position(0.0f, 0.0f, 0.0f, 1.0f)
{
} // Light


Light::~Light()
{
} // ~Light


void Light::SetAmbientColor(float red, float green, float blue, float alpha)
{
	m_ambientColor = XMFLOAT4(red, green, blue, alpha);
	return;
} // SetAmbientColor


void Light::SetDiffuseColor(float red, float green, float blue, float alpha)
{
	m_diffuseColor = XMFLOAT4(red, green, blue, alpha);
	return;
} // SetDiffuseColor


void Light::SetDirection(float x, float y, float z)
{
	m_direction = XMFLOAT3(x, y, z);
	return;
} // SetDirection


void Light::SetSpecularColor(float red, float green, float blue, float alpha)
{
	m_specularColor = XMFLOAT4(red, green, blue, alpha);
	return;
} // SetSpecularColor


void Light::SetSpecularPower(float power)
{
	m_specularPower = power;
	return;
} // SetSpecularPower


void Light::SetPosition(float x, float y, float z)
{
	m_position = XMFLOAT4(x, y, z, 1.0f);
	return;
} // SetPosition


XMFLOAT4 Light::GetAmbientColor()
{
	return m_ambientColor;
} // GetAmbientColor


XMFLOAT4 Light::GetDiffuseColor()
{
	return m_diffuseColor;
} // GetDiffuseColor


XMFLOAT3 Light::GetDirection()
{
	return m_direction;
} // GetDirection


XMFLOAT4 Light::GetSpecularColor()
{
	return m_specularColor;
} // GetSpecularColor


float Light::GetSpecularPower()
{
	return m_specularPower;
} // GetSpecularPower


XMFLOAT4 Light::GetPosition()
{
	return m_position;
} // GetPosition