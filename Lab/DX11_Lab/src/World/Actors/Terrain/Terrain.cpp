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
	m_terrainFilename = nullptr;
	m_colorMapFilename = nullptr;
	m_heightScale = 0;

	m_heightMap = 0;
	m_terrainModel = 0;
	m_TerrainCells = nullptr;
	m_cellCount = 0;
	m_renderCount = 0, m_cellsDrawn = 0, m_cellsCulled = 0;
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
	m_colorMapFilename = nullptr;
	m_heightScale = 0;

	m_heightMap = 0;
	m_terrainModel = 0;
	m_TerrainCells = nullptr;
	m_cellCount = 0;
	m_renderCount = 0, m_cellsDrawn = 0, m_cellsCulled = 0;
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
	//result = LoadBitmapHeightMap();
	//if (!result)
	//{
	//	OutputDebugStringA("Error: LoadBitmapHeightMap 실패!\n");
	//	return false;
	//}

	result = LoadRawHeightMap();
	if (!result)
	{
		OutputDebugStringA("Error: LoadRawHeightMap 실패!\n");
		return false;
	}

	SetTerrainCoordinates();

	result = CalculateNormals();
	if (!result)
	{
		OutputDebugStringA("Error: CalculateNormals 실패!\n");
		return false;
	}

	result = LoadColorMap();
	if (!result)
	{
		OutputDebugStringA("Error: LoadColorMap 실패!\n");
		return false;
	}

	result = BuildTerrainModel();
	if (!result)
	{
		OutputDebugStringA("Error: BuildTerrainModel 실패!\n");
		return false;
	}

	ShutdownHeightMap();

	CalculateTerrainVectors();

	// Create and load the cells with the terrain data.
	result = LoadTerrainCells(device);
	if (!result)
	{
		return false;
	}

	// Release the terrain model now that the terrain cells have been loaded.
	ShutdownTerrainModel();

	//result = InitBuffers(device);
	//if (!result)
	//{
	//	OutputDebugStringA("Error: InitBuffers 실패\n");
	//	return false;
	//}

	ShutdownTerrainModel();

	return true;
} // Init


void Terrain::Shutdown()
{
	ShutdownBuffers();
	ShutdownTerrainModel();
	ShutdownHeightMap();

	//if (m_terrainFilename)
	//{
	//	delete[] m_terrainFilename;
	//	m_terrainFilename = nullptr;
	//}
	return;
} // Shutdown


void Terrain::Frame()
{
	m_renderCount = 0;
	m_cellsDrawn = 0;
	m_cellsCulled = 0;
	return;
} // Frame


bool Terrain::Render(ID3D11DeviceContext* deviceContext)
{
	//RenderBuffers(deviceContext);

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

	m_colorMapFilename = new char[stringLength];
	if (!m_colorMapFilename)
	{
		return false;
	}

	fin.open(filename);
	if (fin.fail())
	{
		MessageBoxA(NULL, filename, "setup file을 여기서 못찾음:", MB_OK);
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

	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}

	// Read in the color map file name.
	fin >> m_colorMapFilename;

	fin.close();
	return true;
} // LoadSetupFile

/*
bool Terrain::LoadBitmapHeightMap()
{
	int error, imageSize, i, j, k, index;
	FILE* filePtr;
	unsigned long long count;
	BITMAPFILEHEADER bitmapFileHeader;
	BITMAPINFOHEADER bitmapInfoHeader;
	unsigned char* bitmapImage;
	unsigned char height;


	// 높이 맵 데이터를 저장할 배열 구조를 생성
	m_heightMap = new HeightMapType[m_terrainWidth * m_terrainHeight];
	if (!m_heightMap)
	{
		return false;
	}

	error = fopen_s(&filePtr, m_terrainFilename, "rb");
	if (error != 0)
	{
		return false;
	}

	count = fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);
	if (count != 1)
	{
		return false;
	}

	count = fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);
	if (count != 1)
	{
		return false;
	}

	if ((bitmapInfoHeader.biHeight != m_terrainHeight) || (bitmapInfoHeader.biWidth != m_terrainWidth))
	{
		return false;
	}

	// 2로 나누어 떨어지지 않는 크기(예: 513x513)를 사용하므로 각 줄에 1바이트를 추가해야 함
	imageSize = m_terrainHeight * ((m_terrainWidth * 3) + 1);

	// 비트맵 이미지 데이터에 필요한 메모리를 할당
	bitmapImage = new unsigned char[imageSize];
	if (!bitmapImage)
	{
		return false;
	}

	// 비트맵 데이터의 시작 부분으로 이동
	fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);

	// 비트맵 이미지 데이터를 읽어들임
	count = fread(bitmapImage, 1, imageSize, filePtr);
	if (count != imageSize)
	{
		return false;
	}

	error = fclose(filePtr);
	if (error != 0)
	{
		return false;
	}

	// 이미지 데이터 버퍼에서 위치를 초기화
	k = 0;

	// 이미지 데이터를 높이 맵 배열에 읽어들임
	for (j = 0; j < m_terrainHeight; j++)
	{
		for (i = 0; i < m_terrainWidth; i++)
		{
			// 비트맵은 상하가 반전되어 있으므로 높이 맵 배열에 아래에서 위로 로드해야 함
			index = (m_terrainWidth * (m_terrainHeight - 1 - j)) + i;

			// 이 위치의 비트맵 이미지 데이터에서 회색조 픽셀 값을 가져옴
			height = bitmapImage[k];

			// 해당 지점의 픽셀 값을 높이 맵 배열의 높이로 저장
			m_heightMap[index].y = (float)height;
			k += 3;
		}

		// 2로 나누어 떨어지지 않는 비트맵(예: 513x513)의 경우, 각 줄 끝에 추가되는 바이트를 보정
		k++;
	}

	delete[] bitmapImage;
	bitmapImage = 0;

	delete[] m_terrainFilename;
	m_terrainFilename = 0;

	return true;
} // LoadBitmapHeightMap
*/

bool Terrain::LoadRawHeightMap()
{
	int error, i, j, index;
	FILE* filePtr;
	unsigned long long imageSize, count;
	unsigned short* rawImage;

	m_heightMap = new HeightMapType[m_terrainWidth * m_terrainHeight];
	if (!m_heightMap)
	{
		return false;
	}

	error = fopen_s(&filePtr, m_terrainFilename, "rb");
	if (error != 0)
	{
		return false;
	}

	imageSize = m_terrainHeight * m_terrainWidth;

	rawImage = new unsigned short[imageSize];
	if (!rawImage)
	{
		return false;
	}

	count = fread(rawImage, sizeof(unsigned short), imageSize, filePtr);
	if (count != imageSize)
	{
		return false;
	}

	error = fclose(filePtr);
	if (error != 0)
	{
		return false;
	}

	// 이미지 데이터를 높이 맵 배열에 복사
	for (j = 0; j < m_terrainHeight; j++)
	{
		for (i = 0; i < m_terrainWidth; i++)
		{
			index = (m_terrainWidth * j) + i;

			// 이 지점의 높이를 높이 맵 배열에 저장
			m_heightMap[index].y = (float)rawImage[index];
		}
	}

	delete[] rawImage;
	rawImage = 0;

	delete[] m_terrainFilename;
	m_terrainFilename = 0;

	return true;
} // LoadRawHeightMap


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
			m_heightMap[index].y /= m_heightScale;
		}
	}

	return;
} // SetTerrainCoordinates


bool Terrain::CalculateNormals()
{
	int i, j, index1, index2, index3, index;
	float vertex1[3], vertex2[3], vertex3[3], vector1[3], vector2[3], sum[3], length;
	VectorType* normals = nullptr;

	normals = new VectorType[(m_terrainHeight - 1) * (m_terrainWidth - 1)];
	if (!normals)
	{
		return false;
	}

	// 메쉬의 모든 면을 순회하며 각 면의 법선을 계산
	for (j = 0; j < (m_terrainHeight - 1); j++)
	{
		for (i = 0; i < (m_terrainWidth - 1); i++)
		{
			index1 = ((j + 1) * m_terrainWidth) + i;      // Bottom left vertex.
			index2 = ((j + 1) * m_terrainWidth) + (i + 1);  // Bottom right vertex.
			index3 = (j * m_terrainWidth) + i;          // Upper left vertex.

			// 해당 면에서 세 개의 꼭짓점을 가져옴
			vertex1[0] = m_heightMap[index1].x;
			vertex1[1] = m_heightMap[index1].y;
			vertex1[2] = m_heightMap[index1].z;

			vertex2[0] = m_heightMap[index2].x;
			vertex2[1] = m_heightMap[index2].y;
			vertex2[2] = m_heightMap[index2].z;

			vertex3[0] = m_heightMap[index3].x;
			vertex3[1] = m_heightMap[index3].y;
			vertex3[2] = m_heightMap[index3].z;

			// 이 면에 대한 두 벡터를 계산
			vector1[0] = vertex1[0] - vertex3[0];
			vector1[1] = vertex1[1] - vertex3[1];
			vector1[2] = vertex1[2] - vertex3[2];
			vector2[0] = vertex3[0] - vertex2[0];
			vector2[1] = vertex3[1] - vertex2[1];
			vector2[2] = vertex3[2] - vertex2[2];

			index = (j * (m_terrainWidth - 1)) + i;

			// 이 두 벡터의 외적을 계산하여 이 얼굴 법선의 정규화되지 않은 값을 얻음
			normals[index].x = (vector1[1] * vector2[2]) - (vector1[2] * vector2[1]);
			normals[index].y = (vector1[2] * vector2[0]) - (vector1[0] * vector2[2]);
			normals[index].z = (vector1[0] * vector2[1]) - (vector1[1] * vector2[0]);

			// 길이를 계산
			length = (float)sqrt((normals[index].x * normals[index].x) + (normals[index].y * normals[index].y) +
				(normals[index].z * normals[index].z));

			// 이 면의 최종 값을 길이를 사용하여 정규화
			normals[index].x = (normals[index].x / length);
			normals[index].y = (normals[index].y / length);
			normals[index].z = (normals[index].z / length);
		}
	}

	// 이제 모든 버텍스를 순회하면서 해당 정점에 접하는 면 법선 벡터들의 합을 구함
	for (j = 0; j < m_terrainHeight; j++)
	{
		for (i = 0; i < m_terrainWidth; i++)
		{
			sum[0] = 0.0f;
			sum[1] = 0.0f;
			sum[2] = 0.0f;

			// Bottom left face
			if (((i - 1) >= 0) && ((j - 1) >= 0))
			{
				index = ((j - 1) * (m_terrainWidth - 1)) + (i - 1);

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
			}

			// Bottom right face
			if ((i < (m_terrainWidth - 1)) && ((j - 1) >= 0))
			{
				index = ((j - 1) * (m_terrainWidth - 1)) + i;

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
			}

			// Upper left face
			if (((i - 1) >= 0) && (j < (m_terrainHeight - 1)))
			{
				index = (j * (m_terrainWidth - 1)) + (i - 1);

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
			}

			// Upper right face
			if ((i < (m_terrainWidth - 1)) && (j < (m_terrainHeight - 1)))
			{
				index = (j * (m_terrainWidth - 1)) + i;

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
			}

			// 법선의 길이를 계산
			length = (float)sqrt((sum[0] * sum[0]) + (sum[1] * sum[1]) + (sum[2] * sum[2]));

			// 높이 맵 배열에서 버텍스 위치에 대한 인덱스를 가져옴
			index = (j * m_terrainWidth) + i;

			// 이 버텍스의 최종 공유 법선을 정규화하고 이를 높이 맵 배열에 저장
			m_heightMap[index].nx = (sum[0] / length);
			m_heightMap[index].ny = (sum[1] / length);
			m_heightMap[index].nz = (sum[2] / length);
		}
	}

	delete[] normals;
	normals = nullptr;

	return true;
} // CalculateNormals


bool Terrain::LoadColorMap()
{
	int error, imageSize, i, j, k, index;
	FILE* filePtr;
	unsigned long long count;
	BITMAPFILEHEADER bitmapFileHeader;
	BITMAPINFOHEADER bitmapInfoHeader;
	unsigned char* bitmapImage;

	error = fopen_s(&filePtr, m_colorMapFilename, "rb");
	if (error != 0)
	{
		return false;
	}

	count = fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);
	if (count != 1)
	{
		return false;
	}

	count = fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);
	if (count != 1)
	{
		return false;
	}

	// 색상 지도의 크기가 지형의 크기와 동일한지 확인하여 1:1 매핑
	if ((bitmapInfoHeader.biWidth != m_terrainWidth) || (bitmapInfoHeader.biHeight != m_terrainHeight))
	{
		return false;
	}

	// 비트맵 이미지 데이터에 필요한 메모리를 할당
	imageSize = m_terrainHeight * ((m_terrainWidth * 3) + 1);

	// Allocate memory for the bitmap image data.
	bitmapImage = new unsigned char[imageSize];
	if (!bitmapImage)
	{
		return false;
	}

	// 비트맵 데이터의 시작 부분으로 이동
	fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);

	// 비트맵 이미지 데이터를 읽어들임
	count = fread(bitmapImage, 1, imageSize, filePtr);
	if (count != imageSize)
	{
		return false;
	}

	error = fclose(filePtr);
	if (error != 0)
	{
		return false;
	}

	// 이미지 데이터 버퍼에서 위치를 초기화
	k = 0;

	// 이미지 데이터를 높이 맵 구조의 색상 맵 부분으로 읽어들임
	for (j = 0; j < m_terrainHeight; j++)
	{
		for (i = 0; i < m_terrainWidth; i++)
		{
			// 비트맵 이미지는 상하가 반전되어 있으므로 배열에 아래에서 위로 로드
			index = (m_terrainWidth * (m_terrainHeight - 1 - j)) + i;

			m_heightMap[index].b = (float)bitmapImage[k] / 255.0f;
			m_heightMap[index].g = (float)bitmapImage[k + 1] / 255.0f;
			m_heightMap[index].r = (float)bitmapImage[k + 2] / 255.0f;

			k += 3;
		}

		// 2로 나누어 떨어지지 않는 비트맵(예: 257x257)의 각 줄 끝에 있는 추가 바이트를 보정
		k++;
	}

	delete[] bitmapImage;
	bitmapImage = 0;

	delete[] m_colorMapFilename;
	m_colorMapFilename = 0;

	return true;
} // LoadColorMap


bool Terrain::BuildTerrainModel()
{
	unsigned int i, j, index, index1, index2, index3, index4;


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
			m_terrainModel[index].tu = 0.0f;
			m_terrainModel[index].tv = 0.0f;
			m_terrainModel[index].nx = m_heightMap[index1].nx;
			m_terrainModel[index].ny = m_heightMap[index1].ny;
			m_terrainModel[index].nz = m_heightMap[index1].nz;
			m_terrainModel[index].r = m_heightMap[index1].r;
			m_terrainModel[index].g = m_heightMap[index1].g;
			m_terrainModel[index].b = m_heightMap[index1].b;
			index++;

			// Triangle 1 - Upper right.
			m_terrainModel[index].x = m_heightMap[index2].x;
			m_terrainModel[index].y = m_heightMap[index2].y;
			m_terrainModel[index].z = m_heightMap[index2].z;
			m_terrainModel[index].tu = 1.0f;
			m_terrainModel[index].tv = 0.0f;
			m_terrainModel[index].nx = m_heightMap[index2].nx;
			m_terrainModel[index].ny = m_heightMap[index2].ny;
			m_terrainModel[index].nz = m_heightMap[index2].nz;
			m_terrainModel[index].r = m_heightMap[index2].r;
			m_terrainModel[index].g = m_heightMap[index2].g;
			m_terrainModel[index].b = m_heightMap[index2].b;
			index++;

			// Triangle 1 - Bottom left.
			m_terrainModel[index].x = m_heightMap[index3].x;
			m_terrainModel[index].y = m_heightMap[index3].y;
			m_terrainModel[index].z = m_heightMap[index3].z;
			m_terrainModel[index].tu = 0.0f;
			m_terrainModel[index].tv = 1.0f;
			m_terrainModel[index].nx = m_heightMap[index3].nx;
			m_terrainModel[index].ny = m_heightMap[index3].ny;
			m_terrainModel[index].nz = m_heightMap[index3].nz;
			m_terrainModel[index].r = m_heightMap[index3].r;
			m_terrainModel[index].g = m_heightMap[index3].g;
			m_terrainModel[index].b = m_heightMap[index3].b;
			index++;

			// Triangle 2 - Bottom left.
			m_terrainModel[index].x = m_heightMap[index3].x;
			m_terrainModel[index].y = m_heightMap[index3].y;
			m_terrainModel[index].z = m_heightMap[index3].z;
			m_terrainModel[index].tu = 0.0f;
			m_terrainModel[index].tv = 1.0f;
			m_terrainModel[index].nx = m_heightMap[index3].nx;
			m_terrainModel[index].ny = m_heightMap[index3].ny;
			m_terrainModel[index].nz = m_heightMap[index3].nz;
			m_terrainModel[index].r = m_heightMap[index3].r;
			m_terrainModel[index].g = m_heightMap[index3].g;
			m_terrainModel[index].b = m_heightMap[index3].b;
			index++;

			// Triangle 2 - Upper right.
			m_terrainModel[index].x = m_heightMap[index2].x;
			m_terrainModel[index].y = m_heightMap[index2].y;
			m_terrainModel[index].z = m_heightMap[index2].z;
			m_terrainModel[index].tu = 1.0f;
			m_terrainModel[index].tv = 0.0f;
			m_terrainModel[index].nx = m_heightMap[index2].nx;
			m_terrainModel[index].ny = m_heightMap[index2].ny;
			m_terrainModel[index].nz = m_heightMap[index2].nz;
			m_terrainModel[index].r = m_heightMap[index2].r;
			m_terrainModel[index].g = m_heightMap[index2].g;
			m_terrainModel[index].b = m_heightMap[index2].b;
			index++;

			// Triangle 2 - Bottom right.
			m_terrainModel[index].x = m_heightMap[index4].x;
			m_terrainModel[index].y = m_heightMap[index4].y;
			m_terrainModel[index].z = m_heightMap[index4].z;
			m_terrainModel[index].tu = 1.0f;
			m_terrainModel[index].tv = 1.0f;
			m_terrainModel[index].nx = m_heightMap[index4].nx;
			m_terrainModel[index].ny = m_heightMap[index4].ny;
			m_terrainModel[index].nz = m_heightMap[index4].nz;
			m_terrainModel[index].r = m_heightMap[index4].r;
			m_terrainModel[index].g = m_heightMap[index4].g;
			m_terrainModel[index].b = m_heightMap[index4].b;
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


void Terrain::CalculateTerrainVectors()
{
	unsigned int faceCount, i, index;
	TempVertexType vertex1, vertex2, vertex3;
	VectorType tangent, binormal;


	// 지형 모델의 면 개수를 계산
	faceCount = m_vertexCount / 3;
	index = 0;

	// 모든 면을 순회하며 접선 벡터, 이중법선 벡터, 법선 벡터를 계산
	for (i = 0; i < faceCount; i++)
	{
		vertex1.x = m_terrainModel[index].x;
		vertex1.y = m_terrainModel[index].y;
		vertex1.z = m_terrainModel[index].z;
		vertex1.tu = m_terrainModel[index].tu;
		vertex1.tv = m_terrainModel[index].tv;
		vertex1.nx = m_terrainModel[index].nx;
		vertex1.ny = m_terrainModel[index].ny;
		vertex1.nz = m_terrainModel[index].nz;
		index++;

		vertex2.x = m_terrainModel[index].x;
		vertex2.y = m_terrainModel[index].y;
		vertex2.z = m_terrainModel[index].z;
		vertex2.tu = m_terrainModel[index].tu;
		vertex2.tv = m_terrainModel[index].tv;
		vertex2.nx = m_terrainModel[index].nx;
		vertex2.ny = m_terrainModel[index].ny;
		vertex2.nz = m_terrainModel[index].nz;
		index++;

		vertex3.x = m_terrainModel[index].x;
		vertex3.y = m_terrainModel[index].y;
		vertex3.z = m_terrainModel[index].z;
		vertex3.tu = m_terrainModel[index].tu;
		vertex3.tv = m_terrainModel[index].tv;
		vertex3.nx = m_terrainModel[index].nx;
		vertex3.ny = m_terrainModel[index].ny;
		vertex3.nz = m_terrainModel[index].nz;
		index++;

		// 해당 면의 접선과 이중법선을 계산
		CalculateTangentBinormal(vertex1, vertex2, vertex3, tangent, binormal);

		// 이 면의 접선 벡터와 이중법선 벡터를 모델 구조에 다시 저장
		m_terrainModel[index - 1].tx = tangent.x;
		m_terrainModel[index - 1].ty = tangent.y;
		m_terrainModel[index - 1].tz = tangent.z;
		m_terrainModel[index - 1].bx = binormal.x;
		m_terrainModel[index - 1].by = binormal.y;
		m_terrainModel[index - 1].bz = binormal.z;

		m_terrainModel[index - 2].tx = tangent.x;
		m_terrainModel[index - 2].ty = tangent.y;
		m_terrainModel[index - 2].tz = tangent.z;
		m_terrainModel[index - 2].bx = binormal.x;
		m_terrainModel[index - 2].by = binormal.y;
		m_terrainModel[index - 2].bz = binormal.z;

		m_terrainModel[index - 3].tx = tangent.x;
		m_terrainModel[index - 3].ty = tangent.y;
		m_terrainModel[index - 3].tz = tangent.z;
		m_terrainModel[index - 3].bx = binormal.x;
		m_terrainModel[index - 3].by = binormal.y;
		m_terrainModel[index - 3].bz = binormal.z;
	}

	return;
} // CalculateTerrainVectors


void Terrain::CalculateTangentBinormal(TempVertexType vertex1, TempVertexType vertex2, TempVertexType vertex3, VectorType& tangent, VectorType& binormal)
{
	float vector1[3], vector2[3];
	float tuVector[2], tvVector[2];
	float den;
	float length;

	vector1[0] = vertex2.x - vertex1.x;
	vector1[1] = vertex2.y - vertex1.y;
	vector1[2] = vertex2.z - vertex1.z;

	vector2[0] = vertex3.x - vertex1.x;
	vector2[1] = vertex3.y - vertex1.y;
	vector2[2] = vertex3.z - vertex1.z;

	tuVector[0] = vertex2.tu - vertex1.tu;
	tvVector[0] = vertex2.tv - vertex1.tv;

	tuVector[1] = vertex3.tu - vertex1.tu;
	tvVector[1] = vertex3.tv - vertex1.tv;

	den = 1.0f / (tuVector[0] * tvVector[1] - tuVector[1] * tvVector[0]);

	tangent.x = (tvVector[1] * vector1[0] - tvVector[0] * vector2[0]) * den;
	tangent.y = (tvVector[1] * vector1[1] - tvVector[0] * vector2[1]) * den;
	tangent.z = (tvVector[1] * vector1[2] - tvVector[0] * vector2[2]) * den;

	binormal.x = (tuVector[0] * vector2[0] - tuVector[1] * vector1[0]) * den;
	binormal.y = (tuVector[0] * vector2[1] - tuVector[1] * vector1[1]) * den;
	binormal.z = (tuVector[0] * vector2[2] - tuVector[1] * vector1[2]) * den;

	length = (float)sqrt((tangent.x * tangent.x) + (tangent.y * tangent.y) + (tangent.z * tangent.z));

	tangent.x = tangent.x / length;
	tangent.y = tangent.y / length;
	tangent.z = tangent.z / length;

	length = (float)sqrt((binormal.x * binormal.x) + (binormal.y * binormal.y) + (binormal.z * binormal.z));

	binormal.x = binormal.x / length;
	binormal.y = binormal.y / length;
	binormal.z = binormal.z / length;

	return;
} // CalculateTangentBinormal


bool Terrain::LoadTerrainCells(ID3D11Device* device)
{
	int cellHeight, cellWidth, cellRowCount, i, j, index;
	bool result;


	cellHeight = 33;
	cellWidth = 33;


	cellRowCount = (m_terrainWidth - 1) / (cellWidth - 1);
	m_cellCount = cellRowCount * cellRowCount;

	m_TerrainCells = new TerrainCell[m_cellCount];
	if (!m_TerrainCells)
	{
		return false;
	}

	// 모든 지형 셀을 순회하며 초기화
	for (j = 0; j < cellRowCount; j++)
	{
		for (i = 0; i < cellRowCount; i++)
		{
			index = (cellRowCount * j) + i;

			result = m_TerrainCells[index].Init(device, m_terrainModel, i, j, cellHeight, cellWidth, m_terrainWidth);
			if (!result)
			{
				return false;
			}
		}
	}

	return true;
} // LoadTerrainCells

/*
bool Terrain::InitBuffers(ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	unsigned int i;


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
		vertices[i].texture = XMFLOAT2(m_terrainModel[i].tu, m_terrainModel[i].tv);
		vertices[i].normal = XMFLOAT3(m_terrainModel[i].nx, m_terrainModel[i].ny, m_terrainModel[i].nz);
		vertices[i].tangent = XMFLOAT3(m_terrainModel[i].tx, m_terrainModel[i].ty, m_terrainModel[i].tz);
		vertices[i].binormal = XMFLOAT3(m_terrainModel[i].bx, m_terrainModel[i].by, m_terrainModel[i].bz);
		vertices[i].color = XMFLOAT3(m_terrainModel[i].r, m_terrainModel[i].g, m_terrainModel[i].b);
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
*/

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

/*
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
*/

void Terrain::ShutdownTerrainCells()
{
	unsigned int i;

	if (m_TerrainCells)
	{
		for (i = 0; i < m_cellCount; i++)
		{
			m_TerrainCells[i].Shutdown();
		}

		delete[] m_TerrainCells;
		m_TerrainCells = 0;
	}

	return;
}


bool Terrain::RenderCell(ID3D11DeviceContext* deviceContext, int cellId)
{
	m_TerrainCells[cellId].Render(deviceContext);
	return true;
}

bool Terrain::RenderCell(ID3D11DeviceContext* deviceContext, int cellId, Frustum* Frustum)
{
	float maxWidth, maxHeight, maxDepth, minWidth, minHeight, minDepth;
	bool result;


	// 지형 셀의 크기를 구함
	m_TerrainCells[cellId].GetCellDimensions(maxWidth, maxHeight, maxDepth, minWidth, minHeight, minDepth);

	// 셀이 보이는지 확인
	// 보이지 않으면 렌더링하지 않고 그대로 반환
	result = Frustum->CheckRectangle2(maxWidth, maxHeight, maxDepth, minWidth, minHeight, minDepth);
	if (!result)
	{
		// 제거된 세포 수를 증가
		m_cellsCulled++;

		return false;
	}

	// 보이는 것이라면 렌더링
	m_TerrainCells[cellId].Render(deviceContext);

	// 셀에 있는 다각형을 렌더링 횟수에 추가.
	m_renderCount += (m_TerrainCells[cellId].GetVertexCount() / 3);

	// 실제로 그려진 셀의 수를 늘림
	m_cellsDrawn++;

	return true;
} // RenderCell


void Terrain::RenderCellLines(ID3D11DeviceContext* deviceContext, int cellId)
{
	m_TerrainCells[cellId].RenderLineBuffers(deviceContext);
	return;
} // RenderCellLines


int Terrain::GetCellIndexCount(int cellId)
{
	return m_TerrainCells[cellId].GetIndexCount();
} // GetCellIndexCount


int Terrain::GetCellLinesIndexCount(int cellId)
{
	return m_TerrainCells[cellId].GetLineBuffersIndexCount();
} // GetCellLinesIndexCount


int Terrain::GetCellCount()
{
	return m_cellCount;
} // GetCellCount


int Terrain::GetRenderCount()
{
	return m_renderCount;
} // GetRenderCount


int Terrain::GetCellsDrawn()
{
	return m_cellsDrawn;
} // GetCellsDrawn


int Terrain::GetCellsCulled()
{
	return m_cellsCulled;
} // GetCellsCulled


bool Terrain::GetHeightAtPosition(float inputX, float inputZ, float& height)
{
	int i, cellId, index;
	float vertex1[3], vertex2[3], vertex3[3];
	bool foundHeight;
	float maxWidth, maxHeight, maxDepth, minWidth, minHeight, minDepth;


	// 모든 지형 셀을 순회하여 inputX와 inputZ가 어느 셀 안에 있는지 확인
	cellId = -1;
	for (i = 0; i < m_cellCount; i++)
	{
		// 현재 셀 크기를 가져옴
		m_TerrainCells[i].GetCellDimensions(maxWidth, maxHeight, maxDepth, minWidth, minHeight, minDepth);

		// 해당 위치들이 이 셀에 있는지 확인
		if ((inputX < maxWidth) && (inputX > minWidth) && (inputZ < maxDepth) && (inputZ > minDepth))
		{
			cellId = i;
			i = m_cellCount;
		}
	}

	// 셀을 찾지 못했다면 입력 위치가 지형 격자 범위를 벗어난 것
	if (cellId == -1)
	{
		return false;
	}

	// 만약 이 셀이 맞는 셀이라면,
	// 이 셀에 있는 모든 삼각형을 확인하여 이 위치에서의 삼각형 높이가 얼마인지 확인.
	for (i = 0; i < (m_TerrainCells[cellId].GetVertexCount() / 3); i++)
	{
		index = i * 3;

		vertex1[0] = m_TerrainCells[cellId].m_vertexList[index].x;
		vertex1[1] = m_TerrainCells[cellId].m_vertexList[index].y;
		vertex1[2] = m_TerrainCells[cellId].m_vertexList[index].z;
		index++;

		vertex2[0] = m_TerrainCells[cellId].m_vertexList[index].x;
		vertex2[1] = m_TerrainCells[cellId].m_vertexList[index].y;
		vertex2[2] = m_TerrainCells[cellId].m_vertexList[index].z;
		index++;

		vertex3[0] = m_TerrainCells[cellId].m_vertexList[index].x;
		vertex3[1] = m_TerrainCells[cellId].m_vertexList[index].y;
		vertex3[2] = m_TerrainCells[cellId].m_vertexList[index].z;

		// 찾고 있는 다각형인지 확인
		foundHeight = CheckHeightOfTriangle(inputX, inputZ, height, vertex1, vertex2, vertex3);
		if (foundHeight)
		{
			return true;
		}
	}

	return false;
} // GetHeightAtPosition


bool Terrain::CheckHeightOfTriangle(float x, float z, float& height, float v0[3], float v1[3], float v2[3])
{
	float startVector[3], directionVector[3], edge1[3], edge2[3], normal[3];
	float Q[3], e1[3], e2[3], e3[3], edgeNormal[3], temp[3];
	float magnitude, D, denominator, numerator, t, determinant;


	// 투사되는 광선의 시작 위치
	startVector[0] = x;
	startVector[1] = 0.0f;
	startVector[2] = z;

	// 광선이 발사되는 방향
	directionVector[0] = 0.0f;
	directionVector[1] = -1.0f;
	directionVector[2] = 0.0f;

	// 주어진 세 점에서 두 변의 길이를 계산
	edge1[0] = v1[0] - v0[0];
	edge1[1] = v1[1] - v0[1];
	edge1[2] = v1[2] - v0[2];

	edge2[0] = v2[0] - v0[0];
	edge2[1] = v2[1] - v0[1];
	edge2[2] = v2[2] - v0[2];

	// 두 변으로부터 삼각형의 법선을 계산
	normal[0] = (edge1[1] * edge2[2]) - (edge1[2] * edge2[1]);
	normal[1] = (edge1[2] * edge2[0]) - (edge1[0] * edge2[2]);
	normal[2] = (edge1[0] * edge2[1]) - (edge1[1] * edge2[0]);

	magnitude = (float)sqrt((normal[0] * normal[0]) + (normal[1] * normal[1]) + (normal[2] * normal[2]));
	normal[0] = normal[0] / magnitude;
	normal[1] = normal[1] / magnitude;
	normal[2] = normal[2] / magnitude;

	// 원점에서 평면까지의 거리
	D = ((-normal[0] * v0[0]) + (-normal[1] * v0[1]) + (-normal[2] * v0[2]));

	// 방정식의 분모
	denominator = ((normal[0] * directionVector[0]) + (normal[1] * directionVector[1]) + (normal[2] * directionVector[2]));

	// 0으로 나누는 오류를 방지하기 위해 결과값이 0에 너무 가까워지지 않도록 주의
	if (fabs(denominator) < 0.0001f)
	{
		return false;
	}

	// 방정식의 분자
	numerator = -1.0f * (((normal[0] * startVector[0]) + (normal[1] * startVector[1]) + (normal[2] * startVector[2])) + D);

	// 두 선분이 삼각형과 만나는 점을 계산
	t = numerator / denominator;

	// 작교벡터
	Q[0] = startVector[0] + (directionVector[0] * t);
	Q[1] = startVector[1] + (directionVector[1] * t);
	Q[2] = startVector[2] + (directionVector[2] * t);

	// 삼각형의 꼭짓점
	e1[0] = v1[0] - v0[0];
	e1[1] = v1[1] - v0[1];
	e1[2] = v1[2] - v0[2];

	e2[0] = v2[0] - v1[0];
	e2[1] = v2[1] - v1[1];
	e2[2] = v2[2] - v1[2];

	e3[0] = v0[0] - v2[0];
	e3[1] = v0[1] - v2[1];
	e3[2] = v0[2] - v2[2];

	// 첫 번째 모서리의 법선을 계산
	edgeNormal[0] = (e1[1] * normal[2]) - (e1[2] * normal[1]);
	edgeNormal[1] = (e1[2] * normal[0]) - (e1[0] * normal[2]);
	edgeNormal[2] = (e1[0] * normal[1]) - (e1[1] * normal[0]);

	// 행렬식을 계산하여 내부, 외부 또는 모서리에 있는지 확인
	temp[0] = Q[0] - v0[0];
	temp[1] = Q[1] - v0[1];
	temp[2] = Q[2] - v0[2];

	determinant = ((edgeNormal[0] * temp[0]) + (edgeNormal[1] * temp[1]) + (edgeNormal[2] * temp[2]));

	// 바깥에 있는지 확인
	if (determinant > 0.001f)
	{
		return false;
	}

	// 두 번째 모서리의 법선을 계산
	edgeNormal[0] = (e2[1] * normal[2]) - (e2[2] * normal[1]);
	edgeNormal[1] = (e2[2] * normal[0]) - (e2[0] * normal[2]);
	edgeNormal[2] = (e2[0] * normal[1]) - (e2[1] * normal[0]);

	// 행렬식을 계산하여 내부, 외부 또는 모서리에 있는지 확인
	temp[0] = Q[0] - v1[0];
	temp[1] = Q[1] - v1[1];
	temp[2] = Q[2] - v1[2];

	determinant = ((edgeNormal[0] * temp[0]) + (edgeNormal[1] * temp[1]) + (edgeNormal[2] * temp[2]));

	// 바깥에 있는지 확인
	if (determinant > 0.001f)
	{
		return false;
	}

	// 세 번째 모서리의 법선을 계산
	edgeNormal[0] = (e3[1] * normal[2]) - (e3[2] * normal[1]);
	edgeNormal[1] = (e3[2] * normal[0]) - (e3[0] * normal[2]);
	edgeNormal[2] = (e3[0] * normal[1]) - (e3[1] * normal[0]);

	// 행렬식을 계산하여 내부, 외부 또는 모서리에 있는지 확인
	temp[0] = Q[0] - v2[0];
	temp[1] = Q[1] - v2[1];
	temp[2] = Q[2] - v2[2];

	determinant = ((edgeNormal[0] * temp[0]) + (edgeNormal[1] * temp[1]) + (edgeNormal[2] * temp[2]));

	// 세 번째 모서리의 법선을 계산
	if (determinant > 0.001f)
	{
		return false;
	}

	height = Q[1];

	return true;
} // CheckHeightOfTriangle