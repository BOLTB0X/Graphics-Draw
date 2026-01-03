////////////////////////////////////////////////////////////////////////////////
// Filename: TextureManager.cpp
////////////////////////////////////////////////////////////////////////////////
#include "TextureManager.h"


TextureManager::TextureManager()
{
	m_TextureArray = 0;
	m_textureCount = 0;
} // TextureManager


TextureManager::TextureManager(const TextureManager& other)
{
	m_TextureArray = 0;
	m_textureCount = 0;
} // TextureManager


TextureManager::~TextureManager()
{
} // ~TextureManager


bool TextureManager::Init(int count)
{
	m_textureCount = count;

	// Create the color texture object.
	m_TextureArray = new Texture[m_textureCount];
	if (!m_TextureArray)
	{
		return false;
	}

	return true;
} // init


void TextureManager::Shutdown()
{
	if (m_TextureArray)
	{
		for (unsigned int i = 0; i < m_textureCount; i++)
		{
			m_TextureArray[i].Shutdown();
		}
		delete[] m_TextureArray;
		m_TextureArray = 0;
	}

	return;
} // Shutdown


bool TextureManager::LoadTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* filename, int location)
{
	bool result;

	result = m_TextureArray[location].Init(device, deviceContext, filename);
	if (!result)
	{
		return false;
	}

	return true;
} // LoadTexture


ID3D11ShaderResourceView* TextureManager::GetTexture(int id)
{
	return m_TextureArray[id].GetTexture();
} // GetTexture
