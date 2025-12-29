////////////////////////////////////////////////////////////////////////////////
// Filename: Texture.cpp
////////////////////////////////////////////////////////////////////////////////
#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Texture::Texture()
{
	imageData = 0;
	m_texture = 0;
	m_textureView = 0;
	m_width = 0;
	m_height = 0;
} // Texture


Texture::Texture(const Texture& other)
{
	imageData = 0;
	m_texture = 0;
	m_textureView = 0;
	m_width = 0;
	m_height = 0;
} // Texture


Texture::~Texture()
{
} // ~Texture


bool Texture::Init(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* filename)
{
	bool result;
	D3D11_TEXTURE2D_DESC textureDesc;
	HRESULT hResult;
	unsigned int rowPitch;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;

	int channels;
	// stbi_set_flip_vertically_on_load(true);
	imageData = stbi_load(filename, &m_width, &m_height, &channels, STBI_rgb_alpha);
	if (!imageData)
	{
		return false;
	}

	// 텍스처 description 셋업
	textureDesc.Height = m_height;
	textureDesc.Width = m_width;
	textureDesc.MipLevels = 0;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

	// 빈 텍스쳐 생성
	hResult = device->CreateTexture2D(&textureDesc, NULL, &m_texture);
	if (FAILED(hResult))
	{
		stbi_image_free(imageData);
		return false;
	}

	rowPitch = (m_width * 4) * sizeof(unsigned char);
	deviceContext->UpdateSubresource(m_texture, 0, NULL, imageData, rowPitch, 0);

	// 셰이더 리소스 뷰 description을 설정
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;

	// 텍스처에 대한 셰이더 리소스 뷰를 생성
	hResult = device->CreateShaderResourceView(m_texture, &srvDesc, &m_textureView);
	if (FAILED(hResult))
	{
		stbi_image_free(imageData);
		return false;
	}

	// 텍스처에 대한 mipmaps을 생성
	deviceContext->GenerateMips(m_textureView);
	stbi_image_free(imageData);

	return true;
} // Init


void Texture::Shutdown()
{
	if (m_textureView)
	{
		m_textureView->Release();
		m_textureView = 0;
	}

	if (m_texture)
	{
		m_texture->Release();
		m_texture = 0;
	}

	if (imageData)
	{
		stbi_image_free(imageData);
		imageData = 0;
	}


	return;
} // Shutdown


ID3D11ShaderResourceView* Texture::GetTexture()
{
	return m_textureView;
} // GetTexture



int Texture::GetWidth()
{
	return m_width;
} // GetWidth


int Texture::GetHeight()
{
	return m_height;
} // GetHeight