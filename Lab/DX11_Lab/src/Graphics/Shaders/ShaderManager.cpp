////////////////////////////////////////////////////////////////////////////////
// Filename: ShaderManager.cpp
////////////////////////////////////////////////////////////////////////////////
#include "ShaderManager.h"
#include <direct.h>

ShaderManager::ShaderManager()
	: m_ColorShader(nullptr),
	m_TextureShader(nullptr),
	m_LightShader(nullptr),
	m_NormalMapShader(nullptr),
	m_TerrainShader(nullptr)
{
} // ShaderManager


ShaderManager::ShaderManager(const ShaderManager& other)
	: m_ColorShader(nullptr),
	m_TextureShader(nullptr),
	m_LightShader(nullptr),
	m_NormalMapShader(nullptr),
	m_TerrainShader(nullptr)
{
} // ShaderManager


ShaderManager::~ShaderManager()
{
} // ShaderManager


bool ShaderManager::Init(ID3D11Device* device, HWND hwnd)
{
	bool result;

	m_ColorShader = new ColorShader;
	result = m_ColorShader->Init(device, hwnd);
	if (!result)
	{
		return false;
	}
	
	m_TextureShader = new TextureShader;

	result = m_TextureShader->Init(device, hwnd);
	if (!result)
	{
		return false;
	}

	m_LightShader = new LightShader;
	result = m_LightShader->Init(device, hwnd);
	if (!result)
	{
		return false;
	}

	m_NormalMapShader = new NormalMapShader;
	result = m_NormalMapShader->Init(device, hwnd);
	if (!result)
	{
		return false;
	}

	m_TerrainShader = new TerrainShader;
	result = m_TerrainShader->Init(device, hwnd);
	if (!result)
	{
		return false;
	}


	return true;
} // Init


void ShaderManager::Shutdown()
{
	if (m_TerrainShader)
	{
		m_TerrainShader->Shutdown();
		delete m_TerrainShader;
		m_TerrainShader = 0;
	}

	if (m_NormalMapShader)
	{
		m_NormalMapShader->Shutdown();
		delete m_NormalMapShader;
		m_NormalMapShader = 0;
	}

	if (m_LightShader)
	{
		m_LightShader->Shutdown();
		delete m_LightShader;
		m_LightShader = 0;
	}

	if (m_TextureShader)
	{
		m_TextureShader->Shutdown();
		delete m_TextureShader;
		m_TextureShader = 0;
	}

	if (m_ColorShader)
	{
		m_ColorShader->Shutdown();
		delete m_ColorShader;
		m_ColorShader = 0;
	}

	return;
} // Shutdown

bool ShaderManager::RenderColorShader(ID3D11DeviceContext* deviceContext, int indexCount, 
	XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix)
{
	bool result;

	result = m_ColorShader->Render(deviceContext, indexCount, worldMatrix, viewMatrix, projectionMatrix);
	if (!result)
	{
		return false;
	}
	return true;
} // RenderColorShader


bool ShaderManager::RenderTextureShader(ID3D11DeviceContext* deviceContext, int indexCount,
	XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix,
	ID3D11ShaderResourceView* texture)
{
	bool result;


	result = m_TextureShader->Render(deviceContext, indexCount, worldMatrix, viewMatrix, projectionMatrix, texture);
	if (!result)
	{
		return false;
	}

	return true;
} // RenderTextureShader


bool ShaderManager::RenderLightShader(ID3D11DeviceContext* deviceContext, int indexCount, 
	XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix,
	ID3D11ShaderResourceView* texture, XMFLOAT3 lightDirection, XMFLOAT4 diffuseColor)
{
	bool result;


	result = m_LightShader->Render(deviceContext, indexCount, 
		worldMatrix, viewMatrix, projectionMatrix, 
		texture, lightDirection, diffuseColor);
	if (!result)
	{
		return false;
	}

	return true;
} // RenderLightShader


bool ShaderManager::RenderNormalMapShader(ID3D11DeviceContext* deviceContext, int indexCount, 
	XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix,
	ID3D11ShaderResourceView* colorTexture, ID3D11ShaderResourceView* normalTexture, XMFLOAT3 lightDirection, XMFLOAT4 diffuseColor)
{
	bool result;


	result = m_NormalMapShader->Render(deviceContext, indexCount, worldMatrix, viewMatrix, projectionMatrix, colorTexture, normalTexture, lightDirection, diffuseColor);
	if (!result)
	{
		return false;
	}

	return true;
} // RenderNormalMapShader

bool ShaderManager::RenderTerrainShader(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
	XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* normalMap,
	XMFLOAT3 lightDirection, XMFLOAT4 diffuseColor)
{
	return m_TerrainShader->Render(deviceContext, indexCount, worldMatrix, viewMatrix, projectionMatrix, texture, normalMap, lightDirection, diffuseColor);
}