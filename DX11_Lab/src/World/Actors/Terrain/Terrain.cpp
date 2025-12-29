////////////////////////////////////////////////////////////////////////////////
// Filename: Terrain.cpp
////////////////////////////////////////////////////////////////////////////////
#include "Terrain.h"


Terrain::Terrain()
{
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
	m_vertexCount = 0;
	m_indexCount = 0;
	m_terrainHeight = 0;
	m_terrainWidth = 0;
	m_terrainFilename = 0;
	m_heightScale = 0;

	m_heightMap = 0;
	m_terrainModel = 0;
} // Terrain


Terrain::Terrain(const Terrain& other)
{
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
	m_vertexCount = 0;
	m_indexCount = 0;
	m_terrainHeight = 0;
	m_terrainWidth = 0;
	m_terrainFilename = 0;
	m_heightScale = 0;

	m_heightMap = 0;
	m_terrainModel = 0;
} // Terrain


Terrain::~Terrain()
{
} // ~Terrain


bool Terrain::Init(ID3D11Device* device, char* setupFilename)
{
	bool result;

	// 설정 파일에서 지형 파일 이름, 크기 등
	result = LoadSetupFile(setupFilename);
	if (!result)
	{
		OutputDebugStringA("Error: Load SetupFile 실패!\n");
		return false;
	}

	// 비트맵 파일의 데이터를 사용하여 지형 높이 맵을 초기화
	result = LoadBitmapHeightMap();
	if (!result)
	{
		OutputDebugStringA("Error: LoadBitmapHeightMap 실패!\n");
		return false;
	}

	SetTerrainCoordinates();
	result = BuildTerrainModel();
	if (!result)
	{
		OutputDebugStringA("Error: BuildTerrainModel Failed!\n");
		return false;
	}

	ShutdownHeightMap();
	result = InitBuffers(device);
	if (!result)
	{
		OutputDebugStringA("Error: InitBuffers Failed!\n");
		return false;
	}

	ShutdownTerrainModel();

	return true;
} // Init


void Terrain::Shutdown()
{
	ShutdownBuffers();
	return;
} // Shutdown


bool Terrain::Render(ID3D11DeviceContext* deviceContext)
{
	RenderBuffers(deviceContext);

	return true;
} // Render


int Terrain::GetIndexCount()
{
	return m_indexCount;
} // GetIndexCount

bool Terrain::LoadSetupFile(char* filename)
{
	int stringLength = 256;
	ifstream fin;
	char input;

	m_terrainFilename = new char[stringLength];
	if (!m_terrainFilename)
	{
		return false;
	}

	fin.open(filename);
	if (fin.fail())
	{
		MessageBoxA(NULL, filename, "Cannot find setup file at:", MB_OK);
		return false;
	}

	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}

	fin >> m_terrainFilename;

	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}
	fin >> m_terrainHeight;

	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}
	fin >> m_terrainWidth;

	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}
	fin >> m_heightScale;

	fin.close();
	return true;
} // LoadSetupFile


bool Terrain::LoadBitmapHeightMap()
{
	int width, height, channels;

	// 하이트맵 하드웨어 데이터(픽셀) 로드
	unsigned char* bitmapImage = stbi_load(m_terrainFilename, &width, &height, &channels, 0);

	if (!bitmapImage) return false;

	// 2. 이미지 크기와 설정 파일의 크기가 일치하는지 확인
	if (width != m_terrainWidth || height != m_terrainHeight)
	{
		OutputDebugStringA("Error: 이미지 크기와 설정 파일의 크기가 일치하지 않음\n");
		stbi_image_free(bitmapImage);
		return false;
	}

	// 3. 하이트맵 배열 메모리 할당
	m_heightMap = new HeightMapType[m_terrainWidth * m_terrainHeight];
	if (!m_heightMap) return false;

	// 4. 이미지 데이터를 m_heightMap 구조체 배열로 옮기기
	int index, k = 0;
	for (int j = 0; j < m_terrainHeight; j++)
	{
		for (int i = 0; i < m_terrainWidth; i++)
		{
			// 래스터텍 방식의 인덱스 계산 (상하 반전 고려 여부에 따라 j 조절 가능)
			index = (m_terrainWidth * (m_terrainHeight - 1 - j)) + i;

			// 픽셀 값(0~255)을 가져와서 높이 스케일을 적용.
			// 채널이 여러 개(RGB)일 경우 첫 번째 채널(R)만 사용
			unsigned char heightValue = bitmapImage[k];

			m_heightMap[index].x = (float)i;
			m_heightMap[index].y = (float)heightValue / 255.0f * m_heightScale; // 높이 정규화 및 스케일링
			m_heightMap[index].z = (float)j;

			k += channels; // 다음 픽셀로 이동
		}
	}

	// 5. 로드된 이미지 메모리 해제 및 파일명 메모리 정리
	stbi_image_free(bitmapImage);

	delete[] m_terrainFilename;
	m_terrainFilename = nullptr;
} // LoadBitmapHeightMap


void Terrain::ShutdownHeightMap()
{
	if (m_heightMap)
	{
		delete[] m_heightMap;
		m_heightMap = 0;
	}

	return;
} // ShutdownHeightMap


void Terrain::SetTerrainCoordinates()
{
	int i, j, index;


	// heightMap 배열의 모든 요소를 ​​순회하면서 각 요소의 좌표를 올바르게 조정
	for (j = 0; j < m_terrainHeight; j++)
	{
		for (i = 0; i < m_terrainWidth; i++)
		{
			index = (m_terrainWidth * j) + i;

			m_heightMap[index].x = (float)i;
			m_heightMap[index].z = -(float)j;

			m_heightMap[index].z += (float)(m_terrainHeight - 1);
			//m_heightMap[index].y /= m_heightScale;
		}
	}

	return;
} // SetTerrainCoordinates


bool Terrain::BuildTerrainModel()
{
	int i, j, index, index1, index2, index3, index4;


	// 3D 지형 모델의 정점 개수를 계산
	m_vertexCount = (m_terrainHeight - 1) * (m_terrainWidth - 1) * 6;

	// 3D 지형 모델 배열을 생성
	m_terrainModel = new ModelType[m_vertexCount];
	if (!m_terrainModel)
	{
		return false;
	}

	// heightMap 배열에 인덱스를 초기화
	index = 0;

	// heightMap 지형 데이터를 사용하여 3D 지형 모델을 불러옴
	// 사각형을 구성하는 네 꼭짓점 각각에 대해 두 개의 삼각형을 만들 것
	for (j = 0; j < (m_terrainHeight - 1); j++)
	{
		for (i = 0; i < (m_terrainWidth - 1); i++)
		{
			index1 = (m_terrainWidth * j) + i;          // Upper left.
			index2 = (m_terrainWidth * j) + (i + 1);      // Upper right.
			index3 = (m_terrainWidth * (j + 1)) + i;      // Bottom left.
			index4 = (m_terrainWidth * (j + 1)) + (i + 1);  // Bottom right.

			// Triangle 1 - Upper left.
			m_terrainModel[index].x = m_heightMap[index1].x;
			m_terrainModel[index].y = m_heightMap[index1].y;
			m_terrainModel[index].z = m_heightMap[index1].z;
			index++;

			// Triangle 1 - Upper right.
			m_terrainModel[index].x = m_heightMap[index2].x;
			m_terrainModel[index].y = m_heightMap[index2].y;
			m_terrainModel[index].z = m_heightMap[index2].z;
			index++;

			// Triangle 1 - Bottom left.
			m_terrainModel[index].x = m_heightMap[index3].x;
			m_terrainModel[index].y = m_heightMap[index3].y;
			m_terrainModel[index].z = m_heightMap[index3].z;
			index++;

			// Triangle 2 - Bottom left.
			m_terrainModel[index].x = m_heightMap[index3].x;
			m_terrainModel[index].y = m_heightMap[index3].y;
			m_terrainModel[index].z = m_heightMap[index3].z;
			index++;

			// Triangle 2 - Upper right.
			m_terrainModel[index].x = m_heightMap[index2].x;
			m_terrainModel[index].y = m_heightMap[index2].y;
			m_terrainModel[index].z = m_heightMap[index2].z;
			index++;

			// Triangle 2 - Bottom right.
			m_terrainModel[index].x = m_heightMap[index4].x;
			m_terrainModel[index].y = m_heightMap[index4].y;
			m_terrainModel[index].z = m_heightMap[index4].z;
			index++;
		}
	}

	return true;
} // BuildTerrainModel


void Terrain::ShutdownTerrainModel()
{
	if (m_terrainModel)
	{
		delete[] m_terrainModel;
		m_terrainModel = 0;
	}

	return;
} // ShutdownTerrainModel


bool Terrain::InitBuffers(ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	int i; /*j, terrainWidth, terrainHeight, index;*/
	XMFLOAT4 color;
	//float positionX, positionZ;


	// 지형 격자의 색상을 설정
	color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	// 지형의 꼭짓점 개수를 계산
	m_vertexCount = (m_terrainWidth - 1) * (m_terrainHeight - 1) * 6;

	// 인덱스 개수를 정점 개수와 동일하게 설정
	m_indexCount = m_vertexCount;

	// 정점 배열을 생성
	vertices = new VertexType[m_vertexCount];
	if (!vertices)
	{
		return false;
	}

	// 인덱스 배열을 생성
	indices = new unsigned long[m_indexCount];
	if (!indices)
	{
		return false;
	}

	// 버텍스 배열과 인덱스 배열에 3D 지형 모델 데이터를 로드
	for (i = 0; i < m_vertexCount; i++)
	{
		vertices[i].position = XMFLOAT3(m_terrainModel[i].x, m_terrainModel[i].y, m_terrainModel[i].z);
		vertices[i].color = color;
		indices[i] = i;
	}


	// 정적 버텍스 버퍼에 대한 설명을 설정
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// 하위 리소스 구조에 정점 데이터에 대한 포인터를 제공
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// 버텍스 버퍼를 생성
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// 버텍스 인덱스 버퍼에 대한 설명을 설정
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// 하위 리소스 구조에 인덱스 데이터에 대한 포인터를 제공
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// 인덱스 버퍼를 생성
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// 해제
	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;

	return true;
} // InitBuffers


void Terrain::ShutdownBuffers()
{
	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}

	return;
} // ShutdownBuffers


void Terrain::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride = sizeof(VertexType);
	unsigned int offset = 0;

	// 입력 어셈블러에서 버텍스 버퍼를 활성화하여 렌더링할 수 있도록 설정
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// 입력 어셈블러에서 인덱스 버퍼를 활성화하여 렌더링할 수 있도록 설정
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// 이 버텍스 버퍼에서 렌더링할 기본 요소의 유형을 설정
	//deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
} // RenderBuffers