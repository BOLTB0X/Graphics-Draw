////////////////////////////////////////////////////////////////////////////////
// Filename: Texture.cpp
////////////////////////////////////////////////////////////////////////////////
#include "Texture.h"


Texture::Texture()
{
	m_targatData = 0;
	m_texture = 0;
	m_textureView = 0;
	m_width = 0;
	m_height = 0;
} // Texture


Texture::Texture(const Texture& other)
{
	m_targatData = 0;
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


	result = LoadTarga32Bit(filename);
	if (!result)
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
		return false;
	}

	rowPitch = (m_width * 4) * sizeof(unsigned char);

	// 타겟이 이미지 데이터를 텍스처에 복사
	deviceContext->UpdateSubresource(m_texture, 0, NULL, m_targatData, rowPitch, 0);

	// 셰이더 리소스 뷰 description을 설정
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;

	// 텍스처에 대한 셰이더 리소스 뷰를 생성
	hResult = device->CreateShaderResourceView(m_texture, &srvDesc, &m_textureView);
	if (FAILED(hResult))
	{
		return false;
	}

	// 텍스처에 대한 mipmaps을 생성
	deviceContext->GenerateMips(m_textureView);

	delete[] m_targatData;
	m_targatData = 0;

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

	if (m_targatData)
	{
		delete[] m_targatData;
		m_targatData = 0;
	}

	return;
} // Shutdown


ID3D11ShaderResourceView* Texture::GetTexture()
{
	return m_textureView;
} // GetTexture


bool Texture::LoadTarga32Bit(char* filename)
{
	int error, bpp, imageSize, index, i, j, k;
	FILE* filePtr;
	unsigned int count;
	TargatHeader targatFileHeader;
	unsigned char* targatImage;


	// 바이너리 형식으로 읽기 위해 Open
	error = fopen_s(&filePtr, filename, "rb");
	if (error != 0)
	{
		return false;
	}

	count = (unsigned int)fread(&targatFileHeader, sizeof(TargatHeader), 1, filePtr);
	if (count != 1)
	{
		return false;
	}

	m_height = (int)targatFileHeader.height;
	m_width = (int)targatFileHeader.width;
	bpp = (int)targatFileHeader.bpp;

	// 24비트가 아닌 32비트인지 확인
	if (bpp != 32)
	{
		return false;
	}

	// 32비트 이미지 데이터의 크기를 계산
	imageSize = m_width * m_height * 4;

	// 이미지 데이터에 필요한 메모리를 할당
	targatImage = new unsigned char[imageSize];

	count = (unsigned int)fread(targatImage, 1, imageSize, filePtr);
	if (count != imageSize)
	{
		return false;
	}

	error = fclose(filePtr);
	if (error != 0)
	{
		return false;
	}

	// 대상 데이터에 대한 메모리를 할당
	m_targatData = new unsigned char[imageSize];
	if (!m_targatData || !targatImage) 
		return false;

	// 데이터 배열의 인덱스를 초기화
	index = 0;

	// 이미지 데이터의 인덱스를 초기화
	k = (m_width * m_height * 4) - (m_width * 4);

	// 타겟이 이미지 데이터를 올바른 순서대로 타르가 대상 배열에 복사
	// 형식은 거꾸로 저장되어 있고 RGBA 순서도 아님
	for (j = 0; j < m_height; j++)
	{
		int readRow = (m_height - 1 - j);

		for (i = 0; i < m_width; i++)
		{
			int readIndex = (readRow * m_width + i) * 4;
			int writeIndex = (j * m_width + i) * 4;

			if (writeIndex + 3 < imageSize && readIndex + 3 < imageSize)
			{
				m_targatData[writeIndex + 0] = targatImage[readIndex + 2]; // R
				m_targatData[writeIndex + 1] = targatImage[readIndex + 1]; // G
				m_targatData[writeIndex + 2] = targatImage[readIndex + 0]; // B
				m_targatData[writeIndex + 3] = targatImage[readIndex + 3]; // A
			}
			//m_targatData[index + 0] = targatImage[k + 2];  // Red.
			//m_targatData[index + 1] = targatImage[k + 1];  // Green.
			//m_targatData[index + 2] = targatImage[k + 0];  // Blue
			//m_targatData[index + 3] = targatImage[k + 3];  // Alpha

			k += 4;
			index += 4;
		}

		// 타겟이 이미지 데이터 인덱스를 해당 열의 시작 부분, 즉 이전 행으로 되돌려야 함
		// 현재 데이터가 거꾸로 읽히고 있음
		k -= (m_width * 8);
	}

	delete[] targatImage;
	targatImage = 0;

	return true;
} // LoadTarga32Bit


int Texture::GetWidth()
{
	return m_width;
} // GetWidth


int Texture::GetHeight()
{
	return m_height;
} // GetHeight